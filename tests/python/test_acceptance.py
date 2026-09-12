"""Offline integration walkthrough on an isolated, self-contained checkout fixture."""
import contextlib
import copy
import io
import json
import os
import shutil
import unittest
from unittest.mock import patch

from test_models import ToolkitFixture, entry, local_reference
import toolkit
from lib import acquire


class AcceptanceTests(ToolkitFixture):
    def setUp(self):
        super().setUp()
        self.item = entry(implemented=True)
        self.item["name"] = 'Offline <sample> "</script>"'
        self.item["integration"]["helpers"] = ["include/toolkit/sample.hpp"]
        self.catalog["entries"] = [self.item, entry("gap")]
        self.write_json("catalog/algorithms.json", self.catalog)
        self.write("include/toolkit/sample.hpp", "#pragma once\ninline int sample(){return 7;}\n")
        self.write("examples/sample.cpp", '#include "toolkit/sample.hpp"\nint main(){return sample()!=7;}\n')
        self.write("tests/cpp/sample_test.cpp",
                   '#include <cassert>\n#include <iostream>\n#include "toolkit/sample.hpp"\n'
                   'int main(){assert(sample()==7);std::cout<<"OK cases=250 seed="<<TOOLKIT_SEED<<"\\n";}\n')
        self.write_json("notebook/profiles/fixture.json", {
            "schema_version": 1, "name": "Fixture notebook", "description": "Local fixture",
            "entries": ["sample"], "include_study_references": False, "print_options": {},
        })

    def cli(self, *args, before=False):
        out, err = io.StringIO(), io.StringIO()
        argv = (["--root", str(self.root), "--json", *args] if before else
                [*args, "--root", str(self.root), "--json"])
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            code = toolkit.main(argv)
        payload = json.loads(out.getvalue())
        self.assertIsInstance(payload["messages"], list)
        return code, payload, err.getvalue()

    def test_exit_codes_json_and_root_position(self):
        for before in (False, True):
            code, data, _ = self.cli("validate", before=before)
            self.assertEqual(code, 0, data)
        code, data, diagnostic = self.cli("test", "--entry", "absent")
        self.assertEqual(code, 2)
        self.assertIn("Unknown", diagnostic)
        code, data, _ = self.cli("test", "--entry", "gap")
        self.assertEqual(code, 1)
        self.assertFalse(data["ok"])
        code, data, _ = self.cli("search", "does not exist")
        self.assertEqual(code, 0)
        self.assertEqual(data["entries"], [])
        code, _, _ = self.cli("sync", "--all", "--source", "fixture")
        self.assertEqual(code, 2)

    @unittest.skipUnless(shutil.which("g++") and shutil.which("git"), "Baseline tools unavailable")
    def test_clean_fixture_offline_test_index_notebook_and_stale_evidence(self):
        with patch("socket.create_connection", side_effect=AssertionError("Unexpected network")), \
             patch.object(acquire, "acquire_sources", side_effect=AssertionError("Implicit source acquisition")):
            for command in (("doctor",), ("validate", "--catalog"), ("search", "example")):
                code, data, _ = self.cli(*command)
                self.assertEqual(code, 0, data)
            code, data, _ = self.cli("test", "--entry", "sample", "--seed", "23", "--record")
            self.assertEqual(code, 0, data)
            code, data, _ = self.cli("search", "sample", "--status", "tested")
            self.assertEqual(code, 0, data)
            self.assertEqual([item["id"] for item in data["entries"]], ["sample"])
            for command in (("index",), ("notebook", "--profile", "notebook/profiles/fixture.json")):
                code, data, _ = self.cli(*command)
                self.assertEqual(code, 0, data)
            expected_paths = ("build/index.html", "build/index.md",
                              "build/notebook/notebook.html", "build/notebook/notebook.md")
            first = {path: (self.root / path).read_bytes() for path in expected_paths}
            self.assertNotIn(b'<sample>', first["build/index.html"])
            self.assertEqual(self.cli("index")[0], 0)
            self.assertEqual(self.cli("notebook", "--profile", "notebook/profiles/fixture.json")[0], 0)
            self.assertEqual(first, {path: (self.root / path).read_bytes() for path in expected_paths})
            self.assertEqual(self.cli("validate", "--links")[0], 0)
            self.write("include/toolkit/sample.hpp", "#pragma once\ninline int sample(){return 8;}\n")
            code, data, _ = self.cli("search", "sample", "--status", "stale")
            self.assertEqual(code, 0, data)
            self.assertEqual([item["id"] for item in data["entries"]], ["sample"])
            self.assertNotEqual(self.cli("notebook", "--profile", "notebook/profiles/fixture.json")[0], 0)
            code, data, _ = self.cli("test", "--entry", "sample", "--seed", "23")
            self.assertEqual(code, 1)
            code, data, _ = self.cli("search", "sample", "--status", "failed")
            self.assertEqual([item["id"] for item in data["entries"]], ["sample"])

    def test_source_validation_is_opt_in_and_always_offline(self):
        with patch.object(acquire, "acquire_sources", return_value=[]) as sync:
            self.assertEqual(self.cli("validate")[0], 0)
            sync.assert_not_called()
            self.assertEqual(self.cli("validate", "--sources")[0], 0)
            sync.assert_called_once_with(self.root, offline=True)

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_catalog_validation_rejects_existing_obsolete_source_pin(self):
        from test_models import source
        upstream = source()
        self.sources["sources"] = [upstream]
        self.write_json("sources/lock.json", self.sources)
        old = f"upstream/fixture/{'b' * 40}/header.hpp"
        self.write(old, "#pragma once\ninline int sample(){return 7;}\n")
        self.write("include/toolkit/sample.hpp", f'#pragma once\n#include "../../{old}"\n')
        for dependency_kind in ("direct", "declared-symlink"):
            with self.subTest(dependency_kind=dependency_kind):
                if dependency_kind == "declared-symlink":
                    helper = self.root / "include/toolkit/sample.hpp"
                    helper.unlink()
                    helper.symlink_to(self.root / old)
                code, data, diagnostic = self.cli("validate", "--catalog")
                self.assertEqual(code, 1, data)
                self.assertIn("pin", diagnostic)

    def test_output_and_profile_traversal_rejected_before_write(self):
        for command in (("index", "--output-dir", "../outside"),
                        ("notebook", "--profile", "../outside.json"),
                        ("index", "--output-dir", "sources")):
            code, _, _ = self.cli(*command)
            self.assertEqual(code, 2)
        self.assertFalse((self.root / "build").exists())

    def test_doctor_missing_tools_is_read_only_failure(self):
        before = sorted(str(path.relative_to(self.root)) for path in self.root.rglob("*"))
        with patch.object(toolkit.shutil, "which", return_value=None):
            code, data, _ = self.cli("doctor")
        self.assertEqual(code, 1)
        self.assertTrue(any("g++" in message["message"] for message in data["messages"]))
        self.assertEqual(before, sorted(str(path.relative_to(self.root)) for path in self.root.rglob("*")))

    def test_selection_profile_dispatch_preserves_order_and_compiler_profile(self):
        self.write_json("notebook/profiles/selection.json", {
            "schema_version": 1, "name": "Ordered test selection", "entries": ["gap", "sample"],
        })
        with patch.object(toolkit.validation, "run_tests", return_value={"ok": True, "records": []}) as run:
            code, data, _ = self.cli("test", "--selection-profile", "notebook/profiles/selection.json",
                                     "--profile", "sanitizers", "--seed", "29", "--record")
        self.assertEqual(code, 0, data)
        run.assert_called_once_with(self.root, entry_ids=["gap", "sample"], all_core=False,
                                    profile="sanitizers", seed=29, record=True)
        self.assertFalse((self.root / "build").exists())

    def test_selection_profile_is_mutually_exclusive_with_entry_and_all_core(self):
        for other in (("--entry", "sample"), ("--all-core",)):
            with self.subTest(other=other), patch.object(toolkit.validation, "run_tests") as run:
                code, _, _ = self.cli("test", "--selection-profile", "notebook/profiles/fixture.json", *other)
                self.assertEqual(code, 2)
                run.assert_not_called()
        self.assertFalse((self.root / "build").exists())

    def test_invalid_selection_profiles_fail_before_reports_or_compilation(self):
        report = {"schema_version": 1, "records": []}
        self.write_json("build/validation.json", report)
        self.write_json("catalog/validation.json", report)
        before = {name: (self.root / name).read_bytes()
                  for name in ("build/validation.json", "catalog/validation.json")}
        for entries in ([], ["unknown"], ["sample", "sample"], ["sample", "unknown"],
                        ["../sample"], ["sample", "sample/../gap"], [None]):
            self.write_json("notebook/profiles/bad.json", {
                "schema_version": 1, "name": "Bad selection", "entries": entries,
            })
            with self.subTest(entries=entries), patch.object(toolkit.validation, "run_tests") as run:
                code, _, _ = self.cli("test", "--selection-profile", "notebook/profiles/bad.json", "--record")
                self.assertEqual(code, 2)
                run.assert_not_called()
        self.write("notebook/profiles/bad.json", '{"schema_version":1,"name":"Bad","entries":["sample"],"entries":[]}')
        self.assertEqual(2, self.cli("test", "--selection-profile", "notebook/profiles/bad.json")[0])
        self.assertEqual(before, {name: (self.root / name).read_bytes() for name in before})
        self.assertFalse((self.root / "build/tests").exists())

    def test_selection_profile_path_rejects_traversal_and_canonical_escape(self):
        profile = self.root / "notebook/profiles/fixture.json"
        alias = self.root / "notebook/profiles/escape.json"
        alias.symlink_to(self.root.parent / "outside.json")
        for path in ("../outside.json", "notebook/profiles/../profiles/fixture.json",
                     "notebook//profiles/fixture.json", "notebook/profiles/escape.json",
                     str(profile), "notebook\\profiles\\fixture.json", "missing.json"):
            with self.subTest(path=path), patch.object(toolkit.validation, "run_tests") as run:
                code, _, _ = self.cli("test", "--selection-profile", path)
                self.assertEqual(code, 2)
                run.assert_not_called()
        self.assertFalse((self.root / "build").exists())

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_selection_profile_runs_only_ordered_entries_with_real_baseline_evidence(self):
        second = copy.deepcopy(self.item)
        second.update(id="second", name="Second")
        self.catalog["entries"].append(second)
        self.write_json("catalog/algorithms.json", self.catalog)
        self.write_json("notebook/profiles/ordered.json", {
            "schema_version": 1, "name": "Ordered", "entries": ["second", "sample"],
        })
        code, data, _ = self.cli("test", "--selection-profile", "notebook/profiles/ordered.json", "--seed", "31")
        self.assertEqual(code, 0, data)
        self.assertEqual(["second", "sample"], [record["entry_id"] for record in data["records"]])
        for record in data["records"]:
            self.assertEqual("baseline", record["profile"])
            self.assertEqual("passed", record["result"])
            self.assertEqual(250, record["case_count"])
            self.assertEqual(31, record["seed"])
        self.assertFalse((self.root / "catalog/validation.json").exists())

    def test_original_prose_cli_search_validation_study_and_failed_test_are_honest(self):
        original = entry("original-guide")
        original.update(implementation="reference-only", local_references=[local_reference()],
                        copy_policy={"status": "permitted", "notices": ["Preserve author credit."]})
        self.catalog["entries"] = [original]
        self.write_json("catalog/algorithms.json", self.catalog)
        path = self.write("docs/techniques/original.md", "# Guide\n## Worked example\nAn original explanation.\n")
        self.write_json("notebook/profiles/study.json", {
            "schema_version": 1, "name": "Original study", "entries": ["original-guide"],
            "include_study_references": True,
        })
        lock = (self.root / "sources/lock.json").read_bytes()
        with patch.object(acquire, "acquire_sources", side_effect=AssertionError("Original prose cannot be synced")):
            code, data, _ = self.cli("validate", "--catalog")
            self.assertEqual(0, code, data)
            code, data, _ = self.cli("search", "original fixture author")
            self.assertEqual(0, code, data)
            self.assertEqual("reference-only", data["entries"][0]["status"])
            self.assertEqual(0, self.cli("index")[0])
            self.assertEqual(0, self.cli("notebook", "--profile", "notebook/profiles/study.json")[0])
            self.assertEqual(0, self.cli("validate", "--links")[0])
            code, data, _ = self.cli("test", "--selection-profile", "notebook/profiles/study.json")
            self.assertEqual(1, code)
            self.assertEqual("failed", data["records"][0]["result"])
            self.assertEqual({}, data["records"][0]["source_pins"])
            self.assertEqual({}, data["records"][0]["dependency_hashes"])
            self.assertEqual("reference-only", self.cli("search", "original-guide")[1]["entries"][0]["status"])
            path.unlink()
            self.assertEqual(2, self.cli("validate", "--catalog")[0])
            code, data, _ = self.cli("search", "original-guide")
            self.assertEqual("unavailable", data["entries"][0]["availability"])
            self.assertEqual("reference-only", data["entries"][0]["status"])
            self.assertEqual(1, self.cli("validate", "--links")[0])
            self.assertEqual(0, self.cli("index", "--output-dir", "build/refreshed-index")[0])
            self.assertEqual(2, self.cli("notebook", "--profile", "notebook/profiles/study.json")[0])
        self.assertEqual(lock, (self.root / "sources/lock.json").read_bytes())


if __name__ == "__main__":
    unittest.main()
