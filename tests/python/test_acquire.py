"""Local Git fixtures intercept only fetch transport; never contact the network."""
from pathlib import Path
import shutil
import subprocess
import unittest
from unittest.mock import patch

from test_models import ToolkitFixture, source
from lib import acquire


@unittest.skipUnless(shutil.which("git"), "Git unavailable")
class AcquireTests(ToolkitFixture):
    def setUp(self):
        super().setUp()
        self.remote = self.root / "fixture-remote"
        self.remote.mkdir()
        self.git(self.remote, "init", "--quiet")
        self.write("fixture-remote/header.hpp", "inline int answer(){return 42;}\n")
        self.write("fixture-remote/.gitignore", "*.generated\n")
        self.git(self.remote, "add", ".")
        self.git(self.remote, "-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid",
                 "commit", "--quiet", "-m", "Fixture content")
        self.item = source()
        self.item["revision"] = self.git(self.remote, "rev-parse", "HEAD")
        self.sources["sources"] = [self.item]
        self.write_json("sources/lock.json", self.sources)
        self.destination = self.root / "upstream" / "fixture" / self.item["revision"]
        self.real_git = acquire._git

    def git(self, directory, *args):
        result = subprocess.run(["git", "-c", "core.hooksPath=/dev/null", "-C", str(directory), *args],
                                capture_output=True, text=True, timeout=20)
        if result.returncode:
            self.fail(result.stderr)
        return result.stdout.strip()

    def local_transport(self, directory, *args):
        if args[0] == "fetch":
            return self.real_git(directory, "-c", "protocol.file.allow=always",
                                 "fetch", "--quiet", "--depth=1", str(self.remote), args[-1])
        return self.real_git(directory, *args)

    def sync_local(self, **kwargs):
        with patch.object(acquire, "_git", side_effect=self.local_transport):
            return acquire.acquire_sources(self.root, **kwargs)

    def test_exact_pin_and_offline_idempotency(self):
        result = self.sync_local()
        self.assertEqual(result[0]["status"], "verified")
        self.assertEqual(self.git(self.destination, "rev-parse", "HEAD"), self.item["revision"])
        self.assertEqual(self.git(self.destination, "config", "--get", "remote.origin.url"),
                         self.item["repository_url"])
        before = (self.destination / "header.hpp").stat().st_mtime_ns
        def no_fetch(directory, *args):
            self.assertNotEqual(args[0], "fetch", "Offline verification attempted network")
            return self.real_git(directory, *args)
        with patch.object(acquire, "_git", side_effect=no_fetch):
            acquire.acquire_sources(self.root, offline=True)
        self.assertEqual(before, (self.destination / "header.hpp").stat().st_mtime_ns)

    def test_dirty_untracked_and_ignored_additions_preserved(self):
        self.sync_local()
        for filename in ("header.hpp", "untracked.txt", "ignored.generated"):
            with self.subTest(filename=filename):
                path = self.destination / filename
                original = path.read_bytes() if path.exists() else None
                path.write_text("preserve my work\n")
                with self.assertRaisesRegex(RuntimeError, "changes|additions|dirty"):
                    acquire.acquire_sources(self.root, offline=True)
                self.assertEqual(path.read_text(), "preserve my work\n")
                if original is None:
                    path.unlink()
                else:
                    path.write_bytes(original)

    def test_wrong_origin_and_wrong_revision_preserved(self):
        self.sync_local()
        self.git(self.destination, "remote", "set-url", "origin", "https://github.com/other/repo")
        with self.assertRaisesRegex(RuntimeError, "origin"):
            acquire.acquire_sources(self.root, offline=True)
        self.assertEqual(self.git(self.destination, "config", "--get", "remote.origin.url"),
                         "https://github.com/other/repo")
        self.git(self.destination, "remote", "set-url", "origin", self.item["repository_url"])
        self.git(self.destination, "-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid",
                 "commit", "--allow-empty", "--quiet", "-m", "Another local fixture pin")
        changed = self.git(self.destination, "rev-parse", "HEAD")
        with self.assertRaisesRegex(RuntimeError, "revision"):
            acquire.acquire_sources(self.root, offline=True)
        self.assertEqual(changed, self.git(self.destination, "rev-parse", "HEAD"))

    def test_unknown_disabled_and_traversal_fail_before_writes(self):
        for selection in (["unknown"], ["fixture", "unknown"], ["fixture", "fixture"], []):
            with self.subTest(selection=selection), self.assertRaises(ValueError):
                acquire.acquire_sources(self.root, source_ids=selection)
        self.item["acquire"] = False
        self.write_json("sources/lock.json", self.sources)
        with self.assertRaises(ValueError):
            acquire.acquire_sources(self.root, source_ids=["fixture"])
        self.item["id"] = "../escape"
        self.write_json("sources/lock.json", self.sources)
        with self.assertRaises(ValueError):
            acquire.acquire_sources(self.root)
        self.assertFalse((self.root / "upstream").exists())

    def test_malformed_lock_and_selection_types_are_input_errors(self):
        malformed = [
            None, [], 1,
            {"schema_version": 1, "sources": {}},
            {"schema_version": True, "sources": []},
            {"schema_version": 1, "sources": [None]},
            {"schema_version": 1, "sources": [{**self.item, "attribution": []}]},
        ]
        for document in malformed:
            with self.subTest(document=document):
                self.write_json("sources/lock.json", document)
                with self.assertRaises(ValueError):
                    acquire.acquire_sources(self.root, dry_run=True)
        self.write_json("sources/lock.json", self.sources)
        for selection in ("fixture", ("fixture",), 1, [None], [["fixture"]]):
            with self.subTest(selection=selection), self.assertRaises(ValueError):
                acquire.acquire_sources(self.root, source_ids=selection, dry_run=True)
        self.assertFalse((self.root / "upstream").exists())

    def test_offline_missing_and_dry_run_never_write(self):
        with self.assertRaisesRegex(RuntimeError, "missing"):
            acquire.acquire_sources(self.root, offline=True)
        result = acquire.acquire_sources(self.root, dry_run=True)
        self.assertEqual(result[0]["status"], "would-acquire")
        self.assertFalse((self.root / "upstream").exists())

    def test_missing_revision_and_failed_fetch_cleanup_only_owned_staging(self):
        self.item["revision"] = "0" * 40
        self.write_json("sources/lock.json", self.sources)
        unrelated = self.write("upstream/.staging/other-writer/keep.txt", "keep")
        with self.assertRaisesRegex(RuntimeError, "not our ref|fetch"):
            self.sync_local()
        self.assertEqual(unrelated.read_text(), "keep")
        self.assertEqual(sorted(p.name for p in (self.root / "upstream/.staging").iterdir()), ["other-writer"])
        self.assertEqual(list((self.root / "upstream/.locks").iterdir()), [])

    def test_symlink_destination_cannot_escape_or_alias(self):
        target = self.root / "unowned"
        target.mkdir()
        (self.root / "upstream").symlink_to(target, target_is_directory=True)
        with self.assertRaises(ValueError):
            acquire.acquire_sources(self.root, dry_run=True)
        self.assertEqual(list(target.iterdir()), [])

    def test_concurrent_writer_lock_and_destination_are_preserved(self):
        lock = self.root / "upstream/.locks" / f"fixture-{self.item['revision']}"
        lock.mkdir(parents=True)
        with self.assertRaisesRegex(RuntimeError, "another acquisition"):
            self.sync_local()
        self.assertTrue(lock.is_dir())
        lock.rmdir()
        def competing_transport(directory, *args):
            result = self.local_transport(directory, *args)
            if args[0] == "checkout":
                self.destination.mkdir(parents=True)
                (self.destination / "keep.txt").write_text("other writer")
            return result
        with patch.object(acquire, "_git", side_effect=competing_transport):
            with self.assertRaisesRegex(RuntimeError, "concurrent destination"):
                acquire.acquire_sources(self.root)
        self.assertEqual((self.destination / "keep.txt").read_text(), "other writer")
        self.assertEqual(list((self.root / "upstream/.staging").iterdir()), [])


if __name__ == "__main__":
    unittest.main()
