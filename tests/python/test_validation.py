import json
from pathlib import Path
import shutil
import subprocess
import unittest
from unittest.mock import patch

from test_models import REPO, ToolkitFixture, entry
from lib import validation


class ValidationTests(ToolkitFixture):
    def setUp(self):
        super().setUp()
        self.item = entry(implemented=True)
        self.catalog["entries"] = [self.item]
        self.write_json("catalog/algorithms.json", self.catalog)
        self.write("include/transitive.hpp", "#pragma once\ninline int answer(){return 42;}\n")
        self.write("include/outer.hpp", '#pragma once\n#include "transitive.hpp"\n')
        self.write("examples/sample.cpp", '#include "outer.hpp"\nint main(){return answer()!=42;}\n')
        self.write("tests/cpp/sample_test.cpp", (
            '#include <cassert>\n#include <iostream>\n#include "outer.hpp"\n'
            'int main(){assert(answer()==42); std::cout<<"OK cases=250 seed="<<TOOLKIT_SEED<<"\\n";}\n'))

    def run_fixture(self, **kwargs):
        return validation.run_tests(self.root, entry_ids=["sample"], **kwargs)

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_real_oracle_recording_and_transitive_staleness(self):
        report = self.run_fixture(seed=19, record=True)
        self.assertTrue(report["ok"], report)
        record = report["records"][0]
        self.assertEqual(record["case_count"], 250)
        self.assertEqual(record["seed"], 19)
        self.assertIn("include/transitive.hpp", record["dependency_hashes"])
        self.assertEqual(validation.entry_status(self.root, self.item), "tested")
        self.assertEqual(json.loads((self.root / "catalog/validation.json").read_text()), report["evidence"])
        self.write("include/transitive.hpp", "#pragma once\ninline int answer(){return 41;}\n")
        self.assertEqual(validation.entry_status(self.root, self.item), "stale")
        report = self.run_fixture(seed=19)
        self.assertFalse(report["ok"])
        self.assertEqual(validation.entry_status(self.root, self.item), "failed")
        self.assertEqual(json.loads((self.root / "catalog/validation.json").read_text())["records"][0]["result"], "passed")
        self.run_fixture(seed=19, record=True)
        self.assertEqual(json.loads((self.root / "catalog/validation.json").read_text())["records"][0]["result"], "failed")

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_compile_error_and_failed_current_override_reviewed_pass(self):
        self.assertTrue(self.run_fixture(record=True)["ok"])
        self.write("tests/cpp/sample_test.cpp", "invalid c++ code\n")
        report = self.run_fixture()
        self.assertFalse(report["ok"])
        self.assertIn("failed", report["records"][0]["failure"].lower())
        self.assertEqual(validation.entry_status(self.root, self.item), "failed")

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_changed_contract_and_shadowed_include_invalidate(self):
        self.assertTrue(self.run_fixture()["ok"])
        changed = json.loads(json.dumps(self.item))
        changed["contract"]["time"] = "Different time bound"
        self.assertEqual(validation.entry_status(self.root, changed), "stale")
        self.write("examples/outer.hpp", "#pragma once\ninline int answer(){return 42;}\n")
        self.assertEqual(validation.entry_status(self.root, self.item), "stale")

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_stale_current_pass_does_not_fall_back_to_matching_history(self):
        original = (self.root / "include/transitive.hpp").read_text()
        self.assertTrue(self.run_fixture(record=True)["ok"])
        self.write("include/transitive.hpp", original + "// Reviewed behavior unchanged; new content\n")
        self.assertTrue(self.run_fixture()["ok"])
        self.write("include/transitive.hpp", original)
        self.assertEqual(validation.entry_status(self.root, self.item), "stale")

    def test_missing_compiler_and_profile_do_not_fallback(self):
        with patch.object(validation.shutil, "which", return_value=None):
            report = self.run_fixture(profile="sanitizers")
        self.assertFalse(report["ok"])
        self.assertIn("g++", report["records"][0]["failure"])
        self.assertEqual(report["records"][0]["profile"], "sanitizers")
        with self.assertRaises(ValueError):
            self.run_fixture(profile="fast-native")

    def test_runtime_timeout_is_explicit(self):
        with patch.object(validation.subprocess, "run", side_effect=subprocess.TimeoutExpired(["test"], 1)):
            with self.assertRaisesRegex(RuntimeError, "timed out"):
                validation.run_process(["test"], self.root, timeout=1)

    def test_selection_and_seed_are_validated_before_writes(self):
        for kwargs in ({"entry_ids": ["unknown"]}, {"entry_ids": ["sample", "sample"]},
                       {"entry_ids": ["sample"], "seed": -1},
                       {"entry_ids": ["sample"], "seed": 2**32}):
            with self.subTest(kwargs=kwargs), self.assertRaises(ValueError):
                validation.run_tests(self.root, **kwargs)
        self.assertFalse((self.root / "build").exists())

    def test_output_symlinks_never_overwrite_source_material(self):
        binary = self.root / "build/tests/baseline/sample-0"
        binary.parent.mkdir(parents=True)
        target = self.root / "examples/sample.cpp"
        before = target.read_bytes()
        binary.symlink_to(target)
        with self.assertRaisesRegex(ValueError, "symlink"):
            self.run_fixture()
        self.assertEqual(target.read_bytes(), before)
        self.assertFalse((self.root / "build/validation.json").exists())

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_dependency_paths_with_spaces_hash_and_dollar(self):
        relative = "include/space # and $/transitive.hpp"
        self.write(relative, "#pragma once\ninline int answer(){return 42;}\n")
        self.write("include/outer.hpp", '#pragma once\n#include "space # and $/transitive.hpp"\n')
        report = self.run_fixture()
        self.assertTrue(report["ok"], report)
        self.assertIn(relative, report["records"][0]["dependency_hashes"])
        self.assertEqual(validation.entry_status(self.root, self.item), "tested")

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_notebook_closure_excludes_test_only_files(self):
        self.write("tests/cpp/test_only.hpp", "#pragma once\n")
        test = self.root / "tests/cpp/sample_test.cpp"
        test.write_text('#include "test_only.hpp"\n' + test.read_text())
        paths = validation.dependency_closure(self.root, self.item)
        self.assertIn("examples/sample.cpp", paths)
        self.assertIn("include/transitive.hpp", paths)
        self.assertNotIn("tests/cpp/sample_test.cpp", paths)
        self.assertNotIn("tests/cpp/test_only.hpp", paths)

    def test_missing_integration_is_explicit_failure(self):
        self.catalog["entries"] = [entry()]
        self.write_json("catalog/algorithms.json", self.catalog)
        report = self.run_fixture()
        self.assertFalse(report["ok"])
        self.assertIn("integration", report["records"][0]["failure"])

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_all_core_uses_explicit_profile_not_study_priority(self):
        self.catalog["entries"].append(entry("study-gap"))
        self.write_json("catalog/algorithms.json", self.catalog)
        self.write_json("notebook/profiles/core.json", {
            "schema_version": 1, "name": "Core", "description": "Exact integration selection",
            "entries": ["sample"], "include_study_references": False, "print_options": {},
        })
        report = validation.run_tests(self.root, all_core=True)
        self.assertTrue(report["ok"], report)
        self.assertEqual([record["entry_id"] for record in report["records"]], ["sample"])

    def test_all_core_requires_explicit_profile_before_writes(self):
        with self.assertRaisesRegex(ValueError, "core.json"):
            validation.run_tests(self.root, all_core=True)
        self.assertFalse((self.root / "build").exists())

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_seed_report_and_assertions_are_required(self):
        self.write("tests/cpp/sample_test.cpp",
                   '#include <iostream>\nint main(){std::cout<<"OK cases=250 seed=99\\n";}\n')
        report = self.run_fixture(seed=1)
        self.assertFalse(report["ok"])
        self.assertIn("seed", report["records"][0]["failure"])
        self.assertNotIn("-DNDEBUG", report["records"][0]["flags"])

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_runner_and_profile_edits_invalidate_reviewed_evidence(self):
        self.assertTrue(self.run_fixture(record=True)["ok"])
        self.write("tools/lib/validation.py", "# Modified checkout runner\n")
        self.assertEqual(validation.entry_status(self.root, self.item), "stale")
        (self.root / "tools/lib/validation.py").unlink()
        self.assertEqual(validation.entry_status(self.root, self.item), "tested")
        with patch.dict(validation.PROFILES, baseline=[*validation.PROFILES["baseline"], "-g"]):
            self.assertEqual(validation.entry_status(self.root, self.item), "stale")

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_source_pin_and_undeclared_upstream_closure_are_checked(self):
        from test_models import source
        upstream = source()
        self.sources["sources"] = [upstream]
        self.write_json("sources/lock.json", self.sources)
        relative = f"upstream/fixture/{upstream['revision']}/header.hpp"
        self.write(relative, "#pragma once\ninline int answer(){return 42;}\n")
        self.write("include/outer.hpp", f'#pragma once\n#include "../{relative}"\n')
        report = self.run_fixture(record=True)
        self.assertTrue(report["ok"], report)
        self.assertIn("fixture", report["records"][0]["source_pins"])
        upstream["revision"] = "b" * 40
        self.write_json("sources/lock.json", self.sources)
        self.assertEqual(validation.entry_status(self.root, self.item), "stale")
        self.assertFalse(self.run_fixture()["ok"])

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_system_classified_local_transitive_headers_are_fingerprinted(self):
        self.item["integration"]["helpers"] = ["include/outer.hpp"]
        self.write_json("catalog/algorithms.json", self.catalog)
        for classification in ("environment", "pragma"):
            with self.subTest(classification=classification):
                outer = '#pragma once\n#include "transitive.hpp"\n'
                if classification == "pragma":
                    outer = "#pragma GCC system_header\n" + outer
                self.write("include/outer.hpp", outer)
                self.write("include/transitive.hpp", "#pragma once\ninline int answer(){return 42;}\n")
                environment = {"CPLUS_INCLUDE_PATH": str(self.root / "include")} if classification == "environment" else {}
                with patch.dict("os.environ", environment):
                    report = self.run_fixture()
                    self.assertTrue(report["ok"], report)
                    self.assertIn("include/transitive.hpp", report["records"][0]["dependency_hashes"])
                    self.write("include/transitive.hpp",
                               "#pragma once\ninline int answer(){return 42;}\n// changed transitive content\n")
                    self.assertEqual(validation.entry_status(self.root, self.item), "stale")

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_symlink_dependency_retains_alias_and_canonical_source_pin(self):
        from test_models import source
        upstream = source()
        self.sources["sources"] = [upstream]
        self.write_json("sources/lock.json", self.sources)
        relative = f"upstream/fixture/{upstream['revision']}/header.hpp"
        target = self.write(relative, "#pragma once\ninline int answer(){return 42;}\n")
        alias = self.root / "include/pin-alias.hpp"
        alias.symlink_to(target)
        self.item["integration"]["helpers"] = ["include/pin-alias.hpp"]
        self.write_json("catalog/algorithms.json", self.catalog)
        self.write("include/outer.hpp", '#pragma once\n#include "pin-alias.hpp"\n')
        report = self.run_fixture()
        self.assertTrue(report["ok"], report)
        record = report["records"][0]
        self.assertIn("include/pin-alias.hpp", record["dependency_hashes"])
        self.assertIn(relative, record["dependency_hashes"])
        self.assertIn("fixture", record["source_pins"])
        self.assertEqual(record["dependency_aliases"]["include/pin-alias.hpp"], str(target))
        alias.unlink()
        alias.symlink_to("../" + relative)
        self.assertEqual(validation.entry_status(self.root, self.item), "stale")
        upstream["revision"] = "b" * 40
        self.write_json("sources/lock.json", self.sources)
        self.assertFalse(self.run_fixture()["ok"])

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_unused_declared_alias_cannot_hide_an_obsolete_pin(self):
        from test_models import source
        upstream = source()
        self.sources["sources"] = [upstream]
        self.write_json("sources/lock.json", self.sources)
        target = self.write(f"upstream/fixture/{'b' * 40}/header.hpp", "// old pinned helper\n")
        alias = self.root / "include/pin-alias.hpp"
        alias.symlink_to(target)
        self.item["integration"]["helpers"] = ["include/pin-alias.hpp"]
        self.write_json("catalog/algorithms.json", self.catalog)
        report = self.run_fixture()
        self.assertFalse(report["ok"], report)
        self.assertIn("pin", report["records"][0]["failure"])

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_directory_alias_cannot_hide_source_pin(self):
        from test_models import source
        upstream = source()
        self.sources["sources"] = [upstream]
        self.write_json("sources/lock.json", self.sources)
        relative = f"upstream/fixture/{upstream['revision']}/header.hpp"
        target = self.write(relative, "#pragma once\ninline int answer(){return 42;}\n")
        alias = self.root / "include/library"
        alias.symlink_to(target.parent, target_is_directory=True)
        self.write("include/outer.hpp", '#pragma once\n#include "library/header.hpp"\n')
        fingerprint = validation.fingerprint_entry(self.root, self.item)
        self.assertIn(relative, fingerprint["dependency_hashes"])
        self.assertIn("include/library/header.hpp", fingerprint["dependency_hashes"])
        self.assertIn("include/library", fingerprint["dependency_aliases"])
        upstream["revision"] = "b" * 40
        self.write_json("sources/lock.json", self.sources)
        with self.assertRaisesRegex(RuntimeError, "pin"):
            validation.fingerprint_entry(self.root, self.item)

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_external_system_marked_project_header_is_rejected(self):
        outside = self.root.parent / (self.root.name + "-external")
        outside.mkdir()
        self.addCleanup(shutil.rmtree, outside)
        header = outside / "project.hpp"
        header.write_text("#pragma GCC system_header\ninline int answer(){return 42;}\n")
        self.write("include/outer.hpp", f'#pragma once\n#include "{header}"\n')
        report = self.run_fixture()
        self.assertFalse(report["ok"], report)
        self.assertIn("external", report["records"][0]["failure"].lower())

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_inherited_project_include_paths_are_not_toolchain_roots(self):
        outside = self.root.parent / (self.root.name + "-external")
        outside.mkdir()
        self.addCleanup(shutil.rmtree, outside)
        (outside / "inherited.hpp").write_text("#pragma once\ninline int answer(){return 42;}\n")
        self.write("include/outer.hpp", '#pragma once\n#include <inherited.hpp>\n')
        with patch.dict("os.environ", {"CPLUS_INCLUDE_PATH": str(outside), "CPATH": str(outside)}):
            compiler = validation.compiler_info(self.root)
            self.assertNotIn(str(outside), compiler["system_include_roots"])
            report = self.run_fixture()
        self.assertFalse(report["ok"], report)

    @unittest.skipUnless(shutil.which("g++"), "GNU C++ compiler unavailable")
    def test_assertion_failure_runs_with_ndebug_environment_ignored(self):
        self.write("tests/cpp/sample_test.cpp",
                   '#include <cassert>\n#include <iostream>\n'
                   'int main(){assert(false); std::cout<<"OK cases=250 seed="<<TOOLKIT_SEED<<"\\n";}\n')
        with patch.dict("os.environ", {"CXXFLAGS": "-DNDEBUG"}):
            report = self.run_fixture()
        self.assertFalse(report["ok"])
        self.assertIn("failed", report["records"][0]["failure"].lower())


if __name__ == "__main__":
    unittest.main()
