import copy
import hashlib
import unittest
from unittest.mock import patch

from tools.lib import catalog, render
from test_catalog import CatalogFixture


class NotebookTests(CatalogFixture):
    def test_order_full_code_transitive_dependencies_and_notices(self):
        self.write(self.upstream / "space #?.hpp", '#include "dependency.hpp"\n// CC0\nint fixture() { return dependency(); }\n')
        second = copy.deepcopy(self.entry)
        second.update(id="second", name="Second")
        self.save([self.entry, second])
        profile = self.profile(["second", "example"])
        output = self.root / "build/notebook"
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            render.generate_notebook(self.root, profile, output)
            first = {p.name: p.read_bytes() for p in output.iterdir()}
            render.generate_notebook(self.root, profile, output)
        self.assertEqual(first, {p.name: p.read_bytes() for p in output.iterdir()})
        text = (output / "notebook.md").read_text()
        self.assertLess(text.index("## Second"), text.index("## Example"))
        self.assertIn("int main()", text)
        self.assertIn("int dependency()", text)
        self.assertEqual(1, text.count("int dependency()"))
        self.assertIn("Fixture author", text)
        self.assertIn("CC0 fixture notice", text)
        self.assertEqual([], render.validate_links(self.root, output))

    def test_untested_failed_stale_and_blocked_rejected(self):
        profile = self.profile()
        for state in ("implemented", "stale", "failed"):
            with patch.object(catalog, "_evidence_status", return_value=state):
                with self.assertRaises(ValueError):
                    render.generate_notebook(self.root, profile, self.root / "build/notebook")
        self.entry["copy_policy"] = {"status": "blocked", "notices": []}
        self.entry["reason"] = "License unverified"
        self.save()
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            with self.assertRaises(ValueError):
                render.generate_notebook(self.root, profile, self.root / "build/notebook")

    def test_unknown_and_duplicate_selection_rejected(self):
        for entries in (["unknown"], ["example", "example"]):
            with self.assertRaises(ValueError):
                render.generate_notebook(self.root, self.profile(entries), self.root / "build/notebook")

    def test_study_reference_requires_opt_in_and_is_labeled(self):
        self.entry.update(implementation="reference-only", integration=None)
        self.entry["references"][0].update(path="article.md", kind="article", license="CC-BY-SA-4.0")
        self.entry["copy_policy"]["notices"] = ["Fixture author; CC-BY-SA-4.0"]
        self.write(self.upstream / "article.md", "# Article\nEducational, not a reusable program.\n")
        self.save()
        output = self.root / "build/notebook"
        with self.assertRaises(ValueError):
            render.generate_notebook(self.root, self.profile(), output)
        render.generate_notebook(self.root, self.profile(study=True), output)
        text = (output / "notebook.md").read_text()
        self.assertIn("Study reference", text)
        self.assertIn("Educational", text)

    def test_extensionless_acl_style_include_closure(self):
        self.write("examples/example.cpp", '#include <atcoder/fixture>\nint main() {}\n')
        self.write(self.upstream / "atcoder/fixture", '#include "atcoder/fixture.hpp"\n')
        self.write(self.upstream / "atcoder/fixture.hpp", '#include "atcoder/internal_fixture"\n')
        self.write(self.upstream / "atcoder/internal_fixture", '#include "atcoder/internal_fixture.hpp"\n')
        self.write(self.upstream / "atcoder/internal_fixture.hpp", "// final dependency\n")
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            render.generate_notebook(self.root, self.profile(), self.root / "build/notebook")
        self.assertIn("// final dependency", (self.root / "build/notebook/notebook.md").read_text())

    def test_unlicensed_transitive_dependency_blocked(self):
        self.source["id"] = "kactl"
        self.source["license"]["declared"] = "NOASSERTION"
        new = self.root / "upstream/kactl" / ("a" * 40)
        new.parent.mkdir()
        self.upstream.rename(new)
        self.upstream = new
        self.entry["references"][0]["source"] = "kactl"
        self.entry["integration"]["include_sources"] = ["kactl"]
        self.write(self.upstream / "space #?.hpp", '#include "dependency.hpp"\n// License: CC0\n')
        self.write(self.upstream / "dependency.hpp", "// No license available\n")
        self.save()
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            with self.assertRaisesRegex(ValueError, "permission|license"):
                render.generate_notebook(self.root, self.profile(), self.root / "build/notebook")

    def test_commented_includes_are_not_dependencies(self):
        self.write("examples/example.cpp", '/*\n#include "nonexistent.hpp"\n*/\n'
                   '// #include "also-missing.hpp"\n'
                   '#include "space #?.hpp"\nint main() {}\n')
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            render.generate_notebook(self.root, self.profile(), self.root / "build/notebook")

    def test_nonliteral_and_unresolved_dependencies_fail(self):
        for include in ('#include HEADER', '#include "absent.hpp"', '#include <atcoder/absent>'):
            self.write("examples/example.cpp", include + "\nint main() {}\n")
            with patch.object(catalog, "_evidence_status", return_value="tested"):
                with self.assertRaisesRegex(ValueError, "include|dependency"):
                    render.generate_notebook(self.root, self.profile(), self.root / "build/notebook")

    def test_article_cannot_override_unknown_source_permission(self):
        self.entry.update(implementation="reference-only", integration=None)
        self.entry["references"][0].update(path="article.md", kind="article", license="CC-BY-SA-4.0")
        self.source["license"]["declared"] = "NOASSERTION"
        self.write(self.upstream / "article.md", "# Unlicensed article\n")
        self.save()
        with self.assertRaisesRegex(ValueError, "license|permission"):
            render.generate_notebook(self.root, self.profile(study=True), self.root / "build/notebook")

    def test_legacy_notebook_bytes_are_unchanged_for_code_and_upstream_study(self):
        expected = {
            False: ("8cb2969781361c6ea141340dd20e18602fe71bb41477920e1fbbbbe2a88ab331",
                    "881829d33ba3fefb4c00ab7f054da0e1a862cf3d2308a847d1a9e10cedcfa835"),
            True: ("ed5e7f012218af2f3209eda0740cee1c89ffbb06a314b4faa7ca9cb2d7411b17",
                   "c570a9e817c00e5a32fd1c2c9a220b0849b2fe9ca0df517925c4034a4d194c73"),
        }
        for study in (False, True):
            if study:
                self.entry.update(implementation="reference-only", integration=None)
                self.entry["references"][0].update(path="article.md", kind="article", license="CC-BY-SA-4.0")
                self.entry["copy_policy"]["notices"] = ["Fixture author; CC-BY-SA-4.0"]
                self.write(self.upstream / "article.md", "# Article\nEducational, not a reusable program.\n")
                self.save()
            with patch.object(catalog, "_evidence_status", return_value="tested"):
                output = self.root / "build/notebook"
                render.generate_notebook(self.root, self.profile(study=study), output)
            actual = tuple(hashlib.sha256((output / name).read_bytes()).hexdigest()
                           for name in ("notebook.html", "notebook.md"))
            self.assertEqual(expected[study], actual)

    def test_original_study_requires_opt_in_and_preserves_prose_notices_and_identity(self):
        path = self.original_article(attribution="Original <author> & contributor")
        prose = "# Original guide\n## Worked example\n<script>not executable</script>\n```cpp\nint x;\n```\n"
        path.write_text(prose)
        output = self.root / "build/notebook"
        with self.assertRaisesRegex(ValueError, "current tested evidence"):
            render.generate_notebook(self.root, self.profile(), output)
        self.assertFalse(output.exists())
        with patch.object(catalog, "_evidence_status", side_effect=AssertionError("Prose is not tested")):
            result = render.generate_notebook(self.root, self.profile(study=True), output)
        self.assertEqual(0, result["dependencies"])
        text = (output / "notebook.md").read_text()
        page = (output / "notebook.html").read_text()
        self.assertIn(prose.rstrip(), text)
        self.assertIn("````markdown", text)
        self.assertIn("Study reference", text)
        self.assertIn("repository\\-original article", text)
        self.assertIn("untested educational material", text)
        self.assertIn("no public license selected", text)
        self.assertIn(hashlib.sha256(path.read_bytes()).hexdigest(), text)
        self.assertIn("Original &lt;author&gt; &amp; contributor", page)
        self.assertIn("Keep the original author notice.", page)
        self.assertNotIn("<script>not executable", page)
        self.assertNotIn("Current local baseline evidence: tested", text)
        self.assertNotIn("pin None", text)
        self.assertNotIn("CC0", text)
        self.assertNotIn("external provenance", text)
        self.assertEqual([], render.validate_links(self.root, output))
        first = {p.name: p.read_bytes() for p in output.iterdir()}
        render.generate_notebook(self.root, self.profile(study=True), output)
        self.assertEqual(first, {p.name: p.read_bytes() for p in output.iterdir()})

    def test_implemented_entry_only_copies_optional_original_prose_with_study_opt_in(self):
        implemented = copy.deepcopy(self.entry)
        self.original_article()
        implemented["local_references"] = self.entry["local_references"]
        self.save([implemented])
        output = self.root / "build/notebook"
        with patch.object(catalog, "_evidence_status", return_value="tested"):
            render.generate_notebook(self.root, self.profile(), output)
            text = (output / "notebook.md").read_text()
            self.assertNotIn("Educational original prose", text)
            render.generate_notebook(self.root, self.profile(study=True), output)
            text = (output / "notebook.md").read_text()
            self.assertIn("Educational original prose", text)
            self.assertIn("untested educational material", text)
            self.assertIn("Current local baseline evidence: tested", text)
            self.assertIn("int fixture()", text)
            self.assertIn("Fixture author", text)
            self.assertIn("CC0 fixture notice", text)

    def test_missing_original_or_bad_fragment_is_not_a_notebook_study_fallback(self):
        path = self.original_article()
        output = self.root / "build/notebook"
        path.write_text("# Missing heading\n")
        with self.assertRaisesRegex(ValueError, "heading"):
            render.generate_notebook(self.root, self.profile(study=True), output)
        path.unlink()
        with self.assertRaisesRegex(ValueError, "unavailable"):
            render.generate_notebook(self.root, self.profile(study=True), output)
        self.assertFalse(output.exists())

    def test_original_reference_does_not_bypass_entry_or_upstream_permissions(self):
        upstream_refs = copy.deepcopy(self.entry["references"])
        self.original_article()
        output = self.root / "build/notebook"
        self.entry["copy_policy"]["status"] = "blocked"
        self.save()
        with self.assertRaisesRegex(ValueError, "copying blocked"):
            render.generate_notebook(self.root, self.profile(study=True), output)
        self.entry["copy_policy"]["status"] = "permitted"
        self.entry["references"] = upstream_refs
        self.source["license"]["declared"] = "NOASSERTION"
        self.save()
        with self.assertRaisesRegex(ValueError, "license|permission"):
            render.generate_notebook(self.root, self.profile(study=True), output)
        self.source["license"]["declared"] = "CC0-1.0"
        self.entry["references"][0]["license"] = "NOASSERTION"
        self.save()
        with self.assertRaisesRegex(ValueError, "license"):
            render.generate_notebook(self.root, self.profile(study=True), output)
        self.assertFalse(output.exists())

    def test_nested_study_notebook_keeps_original_relative_links_inside_source_fences(self):
        path = self.original_article(path="docs/techniques/guide #?.md")
        relative_link = "../../examples/meet-in-the-middle-subset-optimization.cpp"
        example = self.write("examples/meet-in-the-middle-subset-optimization.cpp", "int main() {}\n")
        prose = (
            "# Original guide\n## Worked example\n"
            f"[Standalone example]({relative_link})\n"
            "[Section in this original document](#worked-example)\n"
            f'<a href="{relative_link}">An original HTML link</a>\n'
            "```markdown\n[Literal link syntax](not-an-active-link.md)\n```\n"
        )
        path.write_text(prose)
        output = self.root / "build/custom print/deep/study"
        self.assertEqual(example, (path.parent / relative_link).resolve())
        self.assertFalse((output / relative_link).resolve().exists())
        render.generate_notebook(self.root, self.profile(study=True), output)
        original_link = "../../../../docs/techniques/guide%20%23%3F.md#worked-example"
        for name in ("notebook.html", "notebook.md"):
            generated = output / name
            parsed = render._document_links(generated)
            self.assertIn(original_link, parsed.links)
            self.assertNotIn(relative_link, parsed.links)
            self.assertNotIn("#worked-example", parsed.links)
            self.assertNotIn("not-an-active-link.md", parsed.links)
        text = (output / "notebook.md").read_text()
        self.assertIn(prose.rstrip(), text)
        self.assertIn("````markdown\n" + prose, text)
        self.assertEqual([], render.validate_links(self.root, output))
        first = {p.name: p.read_bytes() for p in output.iterdir()}
        render.generate_notebook(self.root, self.profile(study=True), output)
        self.assertEqual(first, {p.name: p.read_bytes() for p in output.iterdir()})
        markdown = output / "notebook.md"
        markdown.write_text(markdown.read_text().replace(
            original_link, original_link.replace("#worked-example", "#missing-heading")))
        errors = render.validate_links(self.root, output)
        self.assertTrue(any("missing-heading" in error for error in errors), errors)
        self.assertFalse(any(relative_link in error for error in errors), errors)


if __name__ == "__main__":
    unittest.main()
