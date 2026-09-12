import copy
import hashlib
import json
from pathlib import Path
import shutil
import unittest
from unittest.mock import patch
import uuid

from tools.lib import catalog, models
from test_models import local_reference


class CatalogFixture(unittest.TestCase):
    def setUp(self):
        self.root = Path("build/python-fixtures") / uuid.uuid4().hex
        self.root.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.root)
        self.root = self.root.resolve()
        self.source = {
            "id": "fixture", "name": "Fixture", "repository_url": "https://github.com/example/fixture",
            "revision": "a" * 40, "acquire": True, "kind": "reusable-library",
            "inclusion_reason": "Test fixture",
            "attribution": {"status": "verified", "author": "Fixture author", "evidence": []},
            "availability": {"status": "public"},
            "license": {"declared": "CC0-1.0", "evidence": ["LICENSE"], "exceptions": [], "copying": "preserve-exceptions"},
            "portability_notes": [], "submodules": [], "managed_dependencies": [],
        }
        self.upstream = self.root / "upstream/fixture" / ("a" * 40)
        self.upstream.mkdir(parents=True)
        self.write(self.upstream / "LICENSE", "CC0 fixture notice\n")
        self.write(self.upstream / "space #?.hpp", "// CC0\nint fixture() { return 1; }\n")
        self.write(self.upstream / "dependency.hpp", "// CC0\nint dependency() { return 2; }\n")
        self.write("examples/example.cpp", '#include "space #?.hpp"\nint main() { return fixture(); }\n')
        self.write("tests/cpp/example_test.cpp", "int main() {}\n")
        self.entry = {
            "id": "example", "name": "Example <tree>", "aliases": ["2-SAT", "Two SAT"],
            "category": "Graphs", "subcategory": "Connectivity", "tags": ["directed"],
            "priority": "core", "implementation": "implemented", "reason": "Documented notes",
            "references": [{"source": "fixture", "path": "space #?.hpp", "kind": "code", "license": "CC0-1.0"}],
            "integration": {"example": "examples/example.cpp", "helpers": [], "include_sources": ["fixture"], "tests": ["tests/cpp/example_test.cpp"]},
            "contract": {key: "Fixture " + key for key in (
                "purpose", "input", "output", "preconditions", "indexing", "time", "space",
                "numeric_limits", "mutation", "global_state", "recursion", "limitations")},
            "copy_policy": {"status": "permitted", "notices": ["Fixture author; CC0-1.0"]},
        }
        self.save()

    def write(self, path, text):
        path = self.root / path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        return path

    def save(self, entries=None):
        self.write("sources/lock.json", json.dumps({"schema_version": 1, "sources": [self.source]}))
        self.write("catalog/algorithms.json", json.dumps({"schema_version": 1, "entries": entries or [self.entry]}))

    def profile(self, entries=None, study=False):
        return self.write("notebook/profiles/test.json", json.dumps({
            "schema_version": 1, "name": "Test", "description": "Ordered fixture",
            "entries": entries or ["example"], "include_study_references": study,
            "print_options": {},
        }))

    def original_article(self, **overrides):
        reference = local_reference(**overrides)
        self.entry.update(implementation="reference-only", integration=None, references=[],
                          local_references=[reference])
        path = self.write(reference["path"],
                          "# Original guide\n## Worked example\nEducational original prose, not executable code.\n")
        self.entry["copy_policy"] = {"status": "permitted", "notices": ["Keep the original author notice."]}
        self.save()
        return path


class CatalogTests(CatalogFixture):
    def test_search_alias_tags_notes_and_case(self):
        for query in ("2-sat", "TWO sat", "DIRECTED", "documented NOTES", "fixture preconditions"):
            self.assertEqual(["example"], [e["id"] for e in catalog.search(self.root, query)])
        self.assertEqual([], catalog.search(self.root, "not in catalog"))
        self.assertEqual([], catalog.search(self.root, "", category="Strings"))
        self.assertEqual([], catalog.search(self.root, "", priority="advanced"))

    def test_status_never_promotes_reference(self):
        self.entry["implementation"] = "reference-only"
        self.entry["integration"] = None
        self.save()
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            self.assertEqual("reference-only", catalog.catalog_entries(self.root)[0]["status"])

    def test_status_filter_and_failure(self):
        for state in ("implemented", "tested", "failed", "stale"):
            with patch.object(catalog, "_evidence_status", return_value=state):
                self.assertEqual(state, catalog.search(self.root, "", status=state)[0]["status"])

    def test_unavailable_source_separate_from_status(self):
        shutil.rmtree(self.upstream)
        result = catalog.catalog_entries(self.root)[0]
        self.assertEqual("unavailable", result["availability"])
        self.assertNotEqual("tested", result["status"])

    def test_duplicate_rejected_and_individual_counts(self):
        self.save([self.entry, copy.deepcopy(self.entry)])
        with self.assertRaises(ValueError):
            catalog.load_catalog(self.root)

    def test_missing_reason_and_traversal_rejected(self):
        for change in ("missing", "path"):
            entry = copy.deepcopy(self.entry)
            if change == "missing":
                entry.update(implementation="missing", integration=None, references=[], reason="")
            else:
                entry["references"][0]["path"] = "../escape"
            self.save([entry])
            with self.assertRaises(ValueError):
                catalog.load_catalog(self.root)

    def test_original_authorship_identity_search_and_counts_are_not_source_pins(self):
        path = self.original_article()
        lock = (self.root / "sources/lock.json").read_bytes()
        with patch.object(catalog, "_evidence_status", side_effect=AssertionError("Prose is not tested")):
            result = catalog.catalog_entries(self.root)[0]
        location = result["locations"][0]
        self.assertEqual("reference-only", result["status"])
        self.assertEqual("available", result["availability"])
        self.assertEqual("repository-original", location["provenance"])
        self.assertEqual("Original fixture author", location["author"])
        self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), location["content_hash"])
        self.assertEqual("docs/techniques/original.md", location["local_path"])
        for field in ("source", "revision", "license"):
            self.assertNotIn(field, location)
        for query in ("original fixture author", "repository-original", "docs/techniques/original.md",
                      location["content_hash"]):
            self.assertEqual(["example"], [e["id"] for e in catalog.search(self.root, query)])
        self.assertEqual({}, catalog.counts([result])["source"])
        self.assertEqual({"reference-only": 1}, catalog.counts([result])["status"])
        self.assertEqual(lock, (self.root / "sources/lock.json").read_bytes())
        path.write_text(path.read_text() + "\nRevised original explanation.\n")
        changed = catalog.catalog_entries(self.root)[0]
        self.assertNotEqual(location["content_hash"], changed["locations"][0]["content_hash"])
        self.assertEqual("reference-only", changed["status"])

    def test_missing_original_document_is_unavailable_not_a_sync_candidate(self):
        path = self.original_article()
        path.unlink()
        result = catalog.catalog_entries(self.root)[0]
        self.assertEqual("unavailable", result["availability"])
        self.assertEqual("reference-only", result["status"])
        self.assertIsNone(result["locations"][0]["local_path"])
        self.assertIsNone(result["locations"][0]["content_hash"])
        self.assertEqual({}, catalog.counts([result])["source"])
        self.assertEqual([], catalog.search(self.root, "", status="tested"))

    def test_upstream_and_original_locations_keep_distinct_provenance(self):
        upstream = copy.deepcopy(self.entry["references"])
        self.original_article()
        self.entry["references"] = upstream
        self.save()
        result = catalog.catalog_entries(self.root)[0]
        self.assertEqual("fixture", result["locations"][0]["source"])
        self.assertEqual("a" * 40, result["locations"][0]["revision"])
        self.assertNotIn("content_hash", result["locations"][0])
        self.assertEqual("repository-original", result["locations"][1]["provenance"])
        self.assertNotIn("revision", result["locations"][1])
        self.assertEqual({"fixture": 1}, catalog.counts([result])["source"])

    def test_original_reference_on_implementation_does_not_get_a_prose_badge(self):
        original = copy.deepcopy(self.entry)
        path = self.original_article()
        original["local_references"] = self.entry["local_references"]
        self.save([original])
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            result = catalog.catalog_entries(self.root)[0]
            self.assertEqual("tested", result["status"])
            self.assertNotIn("status", result["locations"][-1])
            path.unlink()
            result = catalog.catalog_entries(self.root)[0]
            self.assertEqual("stale", result["status"])
            self.assertEqual("unavailable", result["availability"])


class InventoryTests(unittest.TestCase):
    def test_profiles_have_exact_approved_order_and_core_paths(self):
        root = Path(__file__).resolve().parents[2]
        document = catalog.load_catalog(root)
        entries = {entry["id"]: entry for entry in document["entries"]}
        core = json.loads((root / "notebook/profiles/core.json").read_text())["entries"]
        compact = json.loads((root / "notebook/profiles/compact.json").read_text())["entries"]
        coverage = (root / "specs/001-comprehensive-icpc-toolkit/coverage.md").read_text()
        import re
        expected = re.findall(r"^\| C\d\d \| `([^`]+)`", coverage, re.MULTILINE)
        self.assertEqual(expected, core)
        self.assertEqual(40, len(core))
        self.assertEqual(["dsu", "fenwick", "segtree", "dijkstra", "lca", "max-flow",
                          "bipartite-matching", "prefix-function", "crt", "modular-convolution",
                          "convex-hull", "gaussian-elimination"], compact)
        for identity in core:
            entry = entries[identity]
            self.assertEqual("implemented", entry["implementation"], identity)
            for path in [entry["integration"]["example"], *entry["integration"]["helpers"],
                         *entry["integration"]["tests"]]:
                self.assertTrue((root / path).is_file(), f"{identity}: {path}")

    def test_every_named_broader_target_is_individual(self):
        root = Path(__file__).resolve().parents[2]
        entries = catalog.load_catalog(root)["entries"]
        names = {name.casefold() for entry in entries
                 for name in (entry["name"], *entry["aliases"])}
        coverage = (root / "specs/001-comprehensive-icpc-toolkit/coverage.md").read_text()
        inventory = coverage.split("## Broader Inventory:")[1].split("## Concrete Reference")[0]
        splits = {
            "Queue/stack/deque": ["Queue", "Stack", "Deque"],
            "ordered set/map": ["Ordered set", "Ordered map"],
            "recursion/backtracking": ["Recursion", "Backtracking"],
            "permutation ranking/unranking": ["Permutation ranking", "Permutation unranking"],
            "monotone minima/SMAWK": ["Monotone minima", "SMAWK"],
            "prefix/difference transformations": ["Prefix transformations", "Difference transformations"],
            "multidimensional dominance/CDQ": ["Multidimensional dominance", "CDQ divide-and-conquer"],
        }
        for line in inventory.splitlines():
            if not line.startswith("|") or line.startswith(("| Domain", "|---")):
                continue
            columns = [value.strip() for value in line.strip("|").split("|")]
            for cell in columns[1:]:
                for label in cell.split(";"):
                    for name in splits.get(label.strip(), [label.strip()]):
                        self.assertIn(name.casefold(), names, name)
        self.assertEqual(265, len(entries))

    def test_expanded_profiles_and_original_gap_audit(self):
        root = Path(__file__).resolve().parents[2]
        entries = {entry["id"]: entry for entry in catalog.load_catalog(root)["entries"]}
        core = models.load_profile(root, "notebook/profiles/core.json")["entries"]
        additions = models.load_profile(root, "notebook/profiles/expansion.json")["entries"]
        expanded = models.load_profile(root, "notebook/profiles/expanded.json")["entries"]
        self.assertEqual(36, len(additions))
        self.assertFalse(set(core) & set(additions))
        self.assertEqual(core + additions, expanded)
        self.assertEqual(set(expanded), {key for key, entry in entries.items()
                                        if entry["implementation"] == "implemented"})
        audit = models.read_json(root / "catalog/gap-audit.json")
        self.assertEqual(43, len(audit["entries"]))
        self.assertEqual(43, len({row["entry_id"] for row in audit["entries"]}))
        from collections import Counter
        self.assertEqual({"new-integration": 7, "pinned-reference": 8,
                          "original-reference": 23, "residual-gap": 5},
                         dict(Counter(row["disposition"] for row in audit["entries"])))
        for row in audit["entries"]:
            self.assertEqual("missing", row["original"]["implementation"])
            self.assertTrue(row["original"]["reason"])
            entry = entries[row["entry_id"]]
            self.assertEqual({key: entry.get(key, []) for key in
                              ("implementation", "name", "references", "local_references")},
                             row["final"], row["entry_id"])
        self.assertIn("monotone queue", [name.casefold() for name in entries["monotone-queue"]["aliases"]])
        self.assertIn("digit dp", [name.casefold() for name in entries["digit-dp"]["aliases"]])


if __name__ == "__main__":
    unittest.main()
