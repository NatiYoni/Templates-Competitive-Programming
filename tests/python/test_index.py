import json
from pathlib import Path
import unittest
from unittest.mock import patch

from tools.lib import catalog, render
from test_catalog import CatalogFixture


class IndexTests(CatalogFixture):
    def test_offline_encoded_links_and_determinism(self):
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        first = {p.name: p.read_bytes() for p in output.iterdir()}
        render.generate_index(self.root, output)
        self.assertEqual(first, {p.name: p.read_bytes() for p in output.iterdir()})
        html = (output / "index.html").read_text()
        self.assertIn("space%20%23%3F.hpp", html)
        self.assertIn("Example &lt;tree&gt;", html)
        self.assertEqual([], render.validate_links(self.root, output))
        self.assertNotIn("fetch(", (output / "search.js").read_text())
        self.assertIn('id="search-data"', html)

    def test_html_and_script_terminators_are_escaped(self):
        self.entry["name"] = '</script><img src=x onerror="alert(1)">'
        self.entry["reason"] = "<script>alert(2)</script>"
        self.save()
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        text = (output / "index.html").read_text()
        self.assertNotIn("<img", text)
        self.assertNotIn("<script>alert", text)
        embedded = text.split('<script id="search-data" type="application/json">')[1].split("</script>")[0]
        self.assertEqual(self.entry["name"], json.loads(embedded)[0]["name"])

    def test_unavailable_source_has_no_fake_local_link(self):
        import shutil
        shutil.rmtree(self.upstream)
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        text = (output / "index.html").read_text()
        self.assertIn("unavailable", text)
        self.assertNotIn('href="../../upstream/', text)
        self.assertEqual([], render.validate_links(self.root, output))

    def test_existing_checkout_missing_reference_fails(self):
        (self.upstream / "space #?.hpp").unlink()
        with self.assertRaises(ValueError):
            render.generate_index(self.root, self.root / "build/index")

    def test_broken_anchor_and_escaping_link_fail(self):
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        path = output / "index.html"
        path.write_text(path.read_text() + '<a href="#no-such-anchor">bad</a><a href="../../../../outside">bad</a>')
        errors = render.validate_links(self.root, output)
        self.assertTrue(any("no-such-anchor" in error for error in errors))
        self.assertTrue(any("outside" in error for error in errors))

    def test_output_and_read_errors_are_not_hidden(self):
        with self.assertRaises(ValueError):
            render.generate_index(self.root, self.root / "upstream/output")
        with patch.object(Path, "read_text", side_effect=OSError("fixture unreadable")):
            with self.assertRaisesRegex(ValueError, "Cannot read"):
                render.generate_index(self.root, self.root / "build/index")

    def test_escaping_source_symlink_is_rejected(self):
        path = self.upstream / "space #?.hpp"
        path.unlink()
        path.symlink_to(self.root / "examples/example.cpp")
        with self.assertRaisesRegex(ValueError, "escapes"):
            render.generate_index(self.root, self.root / "build/index")

    def test_link_validation_does_not_rebuild_upstream_websites(self):
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        self.write("build/index/clean/upstream/raw/index.md", "[upstream-site-only](not-reconstructed.html)")
        self.assertEqual([], render.validate_links(self.root, output))

    def test_original_links_authorship_hashes_and_search_data_are_escaped(self):
        author = '</script><img src=x onerror="alert(1)"> & Original author'
        self.original_article(path="docs/techniques/space #?.md", attribution=author)
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        first = {p.name: p.read_bytes() for p in output.iterdir()}
        text = (output / "index.html").read_text()
        self.assertIn("docs/techniques/space%20%23%3F.md#worked-example", text)
        self.assertIn("repository-original article", text)
        self.assertIn("SHA-256 ", text)
        self.assertIn("untested educational material", text)
        self.assertIn("no public license selected", text)
        self.assertIn("&lt;img", text)
        self.assertNotIn("<img", text)
        self.assertNotIn("pin None", text)
        embedded = text.split('<script id="search-data" type="application/json">')[1].split("</script>")[0]
        self.assertIn(author.casefold(), json.loads(embedded)[0]["search_text"])
        self.assertEqual([], render.validate_links(self.root, output))
        render.generate_index(self.root, output)
        self.assertEqual(first, {p.name: p.read_bytes() for p in output.iterdir()})

    def test_missing_original_has_no_fake_link_hash_or_acquisition_instruction(self):
        self.original_article().unlink()
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        for name in ("index.html", "index.md"):
            text = (output / name).read_text()
            self.assertIn("unavailable", text)
            self.assertIn("restore the original document", text)
            self.assertNotIn("run sync", text)
            self.assertNotIn("SHA-256", text)
            self.assertNotIn("../../docs/techniques/original.md", text)
        self.assertEqual([], render.validate_links(self.root, output))

    def test_invalid_original_fragment_fails_before_output_writes(self):
        self.original_article(fragment="nonexistent-heading")
        output = self.root / "build/index"
        with self.assertRaisesRegex(ValueError, "heading"):
            render.generate_index(self.root, output)
        self.assertFalse(output.exists())

    def test_original_links_detect_deleted_headings_and_retargeted_aliases(self):
        path = self.original_article()
        output = self.root / "build/index"
        render.generate_index(self.root, output)
        path.write_text("# Different heading\n")
        self.assertTrue(any("heading" in error for error in render.validate_links(self.root, output)))
        upstream = self.write(self.upstream / "article.md", "# Worked example\n")
        path.unlink()
        path.symlink_to(upstream)
        errors = render.validate_links(self.root, output)
        self.assertTrue(any("escapes docs/techniques" in error for error in errors), errors)
        with self.assertRaisesRegex(ValueError, "escapes"):
            render.generate_index(self.root, output)


if __name__ == "__main__":
    unittest.main()
