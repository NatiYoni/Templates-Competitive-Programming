"""Contract fixtures deliberately live under build/, never the system temp area."""
import copy
import json
from pathlib import Path
import shutil
import sys
import unittest
import uuid

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "tools"))
from lib import models


def source(source_id="fixture"):
    return {
        "id": source_id, "name": "Fixture library",
        "repository_url": "https://github.com/example/library",
        "revision": "a" * 40, "acquire": True, "kind": "reusable-library",
        "inclusion_reason": "Local test fixture",
        "attribution": {"status": "verified", "author": "Fixture", "evidence": []},
        "availability": {"status": "public"},
        "license": {"declared": "CC0-1.0", "evidence": [], "exceptions": []},
        "portability_notes": [], "submodules": [], "managed_dependencies": [],
    }


def entry(entry_id="sample", implemented=False):
    return {
        "id": entry_id, "name": "Sample", "aliases": ["Example"], "category": "Basics",
        "subcategory": "Testing", "tags": ["fixture"], "priority": "core",
        "implementation": "implemented" if implemented else "missing",
        "reason": "" if implemented else "No implementation selected.",
        "references": [], "integration": {
            "example": f"examples/{entry_id}.cpp", "helpers": [],
            "include_sources": [], "tests": [f"tests/cpp/{entry_id}_test.cpp"],
        } if implemented else None,
        "contract": {key: "Documented fixture" for key in (
            "purpose", "input", "output", "preconditions", "indexing", "time", "space",
            "numeric_limits", "mutation", "global_state", "recursion", "limitations")},
        "copy_policy": {"status": "permitted" if implemented else "blocked", "notices": []},
    }


def local_reference(**overrides):
    return {
        "path": "docs/techniques/original.md", "fragment": "worked-example",
        "attribution": "Original fixture author",
        "provenance": "repository-original", "kind": "article", **overrides,
    }


class ToolkitFixture(unittest.TestCase):
    def setUp(self):
        self.root = REPO / "build" / "python-fixtures" / uuid.uuid4().hex
        self.root.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.root)
        self.sources = {"schema_version": 1, "sources": []}
        self.catalog = {"schema_version": 1, "entries": [entry()]}
        self.write_json("sources/lock.json", self.sources)
        self.write_json("catalog/algorithms.json", self.catalog)

    def write(self, path, text):
        target = self.root / path
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(text, encoding="utf-8")
        return target

    def write_json(self, path, data):
        return self.write(path, json.dumps(data))


class ModelTests(ToolkitFixture):
    def test_load_empty_sources_and_missing_catalog_entry(self):
        self.assertEqual(models.load_sources(self.root), self.sources)
        self.assertEqual(models.load_catalog(self.root), self.catalog)

    def test_versions_and_duplicates_rejected(self):
        for version in (0, 2, True, "1"):
            with self.subTest(version=version), self.assertRaises(ValueError):
                models.validate_sources({"schema_version": version, "sources": []})
        with self.assertRaises(ValueError):
            models.validate_sources({"schema_version": 1, "sources": [source(), source()]})
        with self.assertRaises(ValueError):
            models.validate_catalog({"schema_version": 1, "entries": [entry(), entry()]}, self.sources)

    def test_source_transport_pin_and_attribution(self):
        for key, value in (
            ("repository_url", "file:///etc/passwd"),
            ("repository_url", "https://user:pass@github.com/a/b"),
            ("repository_url", "https://github.com/a/../b"),
            ("revision", "main"), ("revision", "A" * 40),
            ("attribution", {"status": "unverified"}), ("acquire", "yes"),
        ):
            item = source()
            item[key] = value
            with self.subTest(key=key, value=value), self.assertRaises(ValueError):
                models.validate_sources({"schema_version": 1, "sources": [item]})

    def test_safe_paths_reject_traversal_and_escaping_symlinks(self):
        for path in ("../escape", "/etc/passwd", "a/../b", "a//b", "a\\b", "./a", "a\nb", ""):
            with self.subTest(path=path), self.assertRaises(ValueError):
                models.safe_path(self.root, path)
        (self.root / "escape").symlink_to(self.root.parent, target_is_directory=True)
        with self.assertRaises(ValueError):
            models.safe_path(self.root, "escape/unowned")
        self.assertEqual(models.safe_path(self.root, "space and #/file.cpp"), self.root / "space and #/file.cpp")

    def test_dangling_references_and_incompatible_states(self):
        item = entry()
        item["references"] = [{"source": "unknown", "path": "header.hpp", "kind": "code", "license": "CC0-1.0"}]
        with self.assertRaises(ValueError):
            models.validate_catalog({"schema_version": 1, "entries": [item]}, self.sources)
        for mutate in (
            lambda item: item.update(implementation="tested"),
            lambda item: item.update(implementation="implemented"),
            lambda item: item.update(reason=""),
            lambda item: item["contract"].pop("time"),
        ):
            item = entry()
            mutate(item)
            with self.assertRaises(ValueError):
                models.validate_catalog({"schema_version": 1, "entries": [item]}, self.sources)

    def test_missing_declared_file(self):
        item = entry(implemented=True)
        with self.assertRaises(ValueError):
            models.validate_catalog({"schema_version": 1, "entries": [item]}, self.sources,
                                    root=self.root, check_paths=True)

    def test_reference_symlink_cannot_escape_its_source_pin(self):
        upstream = source()
        manifest = {"schema_version": 1, "sources": [upstream]}
        item = entry()
        item["implementation"] = "reference-only"
        item["references"] = [{"source": "fixture", "path": "alias.hpp",
                               "kind": "code", "license": "CC0-1.0"}]
        target = self.write("include/local.hpp", "Original local code")
        pinned = self.root / "upstream/fixture" / upstream["revision"]
        pinned.mkdir(parents=True)
        (pinned / "alias.hpp").symlink_to(target)
        with self.assertRaisesRegex(ValueError, "escapes"):
            models.validate_catalog({"schema_version": 1, "entries": [item]}, manifest,
                                    root=self.root, check_paths=True)

    def test_profile_unique_known_entries_and_boolean(self):
        profile = {"schema_version": 1, "name": "Fixture", "description": "",
                   "entries": ["sample"], "include_study_references": False, "print_options": {}}
        self.assertEqual(models.validate_profile(profile, self.catalog), profile)
        for key, value in (("entries", []), ("entries", ["sample", "sample"]), ("entries", ["nope"]),
                           ("include_study_references", "false")):
            bad = copy.deepcopy(profile)
            bad[key] = value
            with self.assertRaises(ValueError):
                models.validate_profile(bad, self.catalog)

    def original_catalog(self, reference=None):
        item = entry()
        item.update(implementation="reference-only", local_references=[
            local_reference() if reference is None else reference])
        return {"schema_version": 1, "entries": [item]}

    def test_original_reference_is_additive_without_a_source_or_license(self):
        data = self.original_catalog()
        self.write("docs/techniques/original.md", "# Article\n## Worked example\nOriginal prose.\n")
        self.assertEqual(data, models.validate_catalog(data, self.sources, self.root, check_paths=True))
        ref = data["entries"][0]["local_references"][0]
        for field in ("source", "revision", "license"):
            self.assertNotIn(field, ref)
        ref.pop("fragment")
        models.validate_catalog(data, self.sources, self.root, check_paths=True)
        data["entries"][0]["local_references"] = []
        with self.assertRaisesRegex(ValueError, "needs references"):
            models.validate_catalog(data, self.sources, self.root)

    def test_original_reference_schema_rejects_invented_or_incomplete_provenance(self):
        for fields in (
            {"attribution": ""}, {"attribution": " \n"}, {"attribution": {"author": "Someone"}},
            {"provenance": "upstream"}, {"provenance": None}, {"kind": "code"},
            {"source": "fixture"}, {"revision": "a" * 40}, {"license": "MIT"},
            {"fragment": ""}, {"fragment": "#worked-example"}, {"fragment": "../escape"},
            {"fragment": "heading?x=1"}, {"fragment": None}, {"fragment": "two words"},
        ):
            with self.subTest(fields=fields), self.assertRaises(ValueError):
                models.validate_catalog(self.original_catalog(local_reference(**fields)), self.sources)
        for value in (None, {}, "article", [None]):
            data = self.original_catalog()
            data["entries"][0]["local_references"] = value
            with self.subTest(value=value), self.assertRaises(ValueError):
                models.validate_catalog(data, self.sources)

    def test_original_reference_paths_are_confined_even_without_path_checks(self):
        for path in (
            "/docs/techniques/original.md", "docs/techniques/../original.md",
            "docs/techniques//original.md", "./docs/techniques/original.md",
            "docs\\techniques\\original.md", "docs/techniques/original.md\n",
            "upstream/fixture/article.md", "docs/article.md", "docs/techniques2/article.md",
            "docs/techniques/original.cpp", "docs/techniques/original.md#heading",
        ):
            with self.subTest(path=path), self.assertRaises(ValueError):
                models.validate_catalog(self.original_catalog(local_reference(path=path)), self.sources)

    def test_original_missing_file_and_heading_are_checked_without_acquisition(self):
        data = self.original_catalog()
        models.validate_catalog(data, self.sources, self.root)
        with self.assertRaisesRegex(ValueError, "Missing original"):
            models.validate_catalog(data, self.sources, self.root, check_paths=True)
        for text in ("```markdown\n## Worked example\n```\n", "<!--\n## Worked example\n-->\n",
                     "    ## Worked example\n"):
            path = self.write("docs/techniques/original.md", text)
            with self.subTest(text=text), self.assertRaisesRegex(ValueError, "heading"):
                models.validate_catalog(data, self.sources, self.root, check_paths=True)
        path.unlink()
        path.mkdir()
        with self.assertRaisesRegex(ValueError, "Missing original"):
            models.validate_catalog(data, self.sources, self.root, check_paths=True)

    def test_original_heading_fragments_match_duplicates_and_setext(self):
        text = "# Article\n## Worked **example**\n## Worked **example**\nBoundary cases\n---\n"
        self.write("docs/techniques/original.md", text)
        for fragment in ("worked-example", "worked-example-1", "boundary-cases"):
            data = self.original_catalog(local_reference(fragment=fragment))
            models.validate_catalog(data, self.sources, self.root, check_paths=True)
        self.assertNotIn("fake", models.markdown_heading_anchors("```md\n```` not a close\n# Fake\n```\n"))

    def test_original_symlinks_cannot_borrow_upstream_or_external_authorship(self):
        directory = self.root / "docs/techniques"
        directory.mkdir(parents=True)
        upstream = self.write("upstream/fixture/article.md", "# Worked example\n")
        local = self.write("docs/techniques/actual.md", "# Worked example\n")
        code = self.write("docs/techniques/code.hpp", "# Worked example\n")
        alias = directory / "original.md"
        for target in (upstream, self.root.parent / "external.md", code):
            alias.symlink_to(target)
            with self.subTest(target=target), self.assertRaises(ValueError):
                models.validate_catalog(self.original_catalog(), self.sources, self.root)
            alias.unlink()
        alias.symlink_to(local)
        models.validate_catalog(self.original_catalog(), self.sources, self.root, check_paths=True)
        alias.unlink()
        alias.symlink_to(alias.name)
        with self.assertRaisesRegex(ValueError, "resolve"):
            models.validate_catalog(self.original_catalog(), self.sources, self.root)

    def test_original_directory_and_root_aliases_are_confined(self):
        upstream = self.write("upstream/fixture/article.md", "# Worked example\n")
        directory = self.root / "docs/techniques"
        directory.mkdir(parents=True)
        (directory / "alias").symlink_to(upstream.parent, target_is_directory=True)
        with self.assertRaisesRegex(ValueError, "escapes"):
            models.validate_catalog(self.original_catalog(
                local_reference(path="docs/techniques/alias/article.md")), self.sources, self.root)
        (directory / "alias").unlink()
        directory.rmdir()
        directory.symlink_to(upstream.parent, target_is_directory=True)
        with self.assertRaisesRegex(ValueError, "escapes"):
            models.validate_catalog(self.original_catalog(
                local_reference(path="docs/techniques/article.md")), self.sources, self.root)
        directory.unlink()
        directory.parent.rmdir()
        directory.parent.symlink_to(self.root / "upstream", target_is_directory=True)
        with self.assertRaisesRegex(ValueError, "escapes"):
            models.validate_catalog(self.original_catalog(), self.sources, self.root)

    def test_evidence_requires_hash_and_unique_profile(self):
        record = {"entry_id": "sample", "profile": "baseline", "result": "passed",
                  "fingerprint": "b" * 64, "dependency_hashes": {"examples/sample.cpp": "c" * 64}}
        data = {"schema_version": 1, "records": [record]}
        self.assertEqual(models.validate_evidence(data), data)
        for records in ([record, record], [{**record, "fingerprint": "bad"}],
                        [{**record, "dependency_hashes": {"../outside": "c" * 64}}]):
            with self.assertRaises(ValueError):
                models.validate_evidence({"schema_version": 1, "records": records})

    def test_duplicate_json_keys_fail(self):
        self.write("sources/lock.json", '{"schema_version":1,"schema_version":1,"sources":[]}')
        with self.assertRaises(ValueError):
            models.load_sources(self.root)


if __name__ == "__main__":
    unittest.main()
