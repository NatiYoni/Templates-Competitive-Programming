"""Exact-revision, non-destructive acquisition; no upstream code is executed."""

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


def _contained(root: Path, *parts: str) -> Path:
    root = root.resolve()
    path = root.joinpath(*parts)
    if not path.resolve().is_relative_to(root):
        raise ValueError(f"Path escapes toolkit root: {path}")
    current = root
    for component in path.relative_to(root).parts:
        current = current / component
        if current.is_symlink():
            raise ValueError(f"Symlink is not an acquisition destination: {current}")
    return path


def _git(directory: Path, *args: str) -> str:
    command = ["git", "-c", "core.hooksPath=/dev/null",
               "-c", "protocol.file.allow=never", "-C", str(directory), *args]
    try:
        result = subprocess.run(command, capture_output=True, text=True,
                                timeout=180,
                                env={**os.environ, "GIT_TERMINAL_PROMPT": "0"})
    except FileNotFoundError as exc:
        raise RuntimeError("Git is missing; install Git before acquisition.") from exc
    except subprocess.TimeoutExpired as exc:
        raise RuntimeError(f"Git timed out in {directory}: {args[0]}") from exc
    if result.returncode:
        raise RuntimeError(f"Git {args[0]} failed in {directory}: {result.stderr.strip()}")
    return result.stdout.strip()


def _sources(root: Path) -> list[dict]:
    path = _contained(root, "sources", "lock.json")
    with path.open(encoding="utf-8") as stream:
        data = json.load(stream)
    if (not isinstance(data, dict) or type(data.get("schema_version")) is not int
            or data["schema_version"] != 1 or not isinstance(data.get("sources"), list)):
        raise ValueError(f"Unsupported source lock schema: {path}")
    seen = set()
    for source in data["sources"]:
        if not isinstance(source, dict):
            raise ValueError(f"Source entries must be objects: {path}")
        source_id = source.get("id", "")
        if not isinstance(source_id, str) or not re.fullmatch(r"[a-z0-9]+(?:-[a-z0-9]+)*", source_id):
            raise ValueError(f"Invalid source ID: {source_id!r}")
        if source_id in seen:
            raise ValueError(f"Duplicate source ID: {source_id}")
        seen.add(source_id)
        if not isinstance(source.get("acquire"), bool):
            raise ValueError(f"{source_id}: acquire must be a boolean")
        if source["acquire"]:
            url, revision = source.get("repository_url", ""), source.get("revision", "")
            if not isinstance(url, str) or not re.fullmatch(r"https://github\.com/[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", url):
                raise ValueError(f"{source_id}: expected canonical HTTPS GitHub repository URL")
            if not isinstance(revision, str) or not re.fullmatch(r"[0-9a-f]{40}", revision):
                raise ValueError(f"{source_id}: expected a full lowercase commit SHA")
            attribution = source.get("attribution")
            if not isinstance(attribution, dict) or attribution.get("status") != "verified":
                raise ValueError(f"{source_id}: acquisition needs verified attribution")
    return data["sources"]


def verify_checkout(path: Path, source: dict) -> dict:
    if not path.is_dir() or not (path / ".git").is_dir() or (path / ".git").is_symlink():
        raise RuntimeError(f"{source['id']}: not an attributable standalone checkout: {path}")
    if Path(_git(path, "rev-parse", "--show-toplevel")).resolve() != path.resolve():
        raise RuntimeError(f"{source['id']}: unexpected repository root at {path}")
    origin = _git(path, "config", "--get", "remote.origin.url")
    if origin != source["repository_url"]:
        raise RuntimeError(f"{source['id']}: origin mismatch at {path}; preserving existing files")
    revision = _git(path, "rev-parse", "HEAD")
    if revision != source["revision"]:
        raise RuntimeError(f"{source['id']}: revision mismatch at {path}; no reset performed")
    dirty = _git(path, "status", "--porcelain", "--untracked-files=all",
                 "--ignored=matching", "--ignore-submodules=none")
    if dirty:
        raise RuntimeError(f"{source['id']}: checkout has local changes/additions at {path}:\n{dirty}")
    gitlinks = []
    for line in _git(path, "ls-tree", "-r", "HEAD").splitlines():
        if line.startswith("160000 "):
            metadata, name = line.split("\t", 1)
            gitlinks.append({"path": name, "revision": metadata.split()[2],
                             "status": "not-initialized"})
    return {"id": source["id"], "revision": revision, "path": str(path),
            "status": "verified", "submodules": gitlinks}


def acquire_sources(root: Path, source_ids: list[str] | None = None,
                    offline: bool = False, dry_run: bool = False) -> list[dict]:
    root = Path(root).resolve()
    all_sources = _sources(root)
    by_id = {source["id"]: source for source in all_sources}
    if source_ids is None:
        sources = [source for source in all_sources if source["acquire"]]
    else:
        if (not isinstance(source_ids, list) or not source_ids
                or any(not isinstance(source_id, str) for source_id in source_ids)
                or len(set(source_ids)) != len(source_ids)):
            raise ValueError("Select at least one unique source ID")
        sources = []
        for source_id in source_ids:
            if source_id not in by_id or not by_id[source_id]["acquire"]:
                raise ValueError(f"Unknown or disabled source: {source_id}")
            sources.append(by_id[source_id])
    destinations = []
    for source in sources:
        destination = _contained(root, "upstream", source["id"], source["revision"])
        if destination.exists():
            verify_checkout(destination, source)
        elif offline:
            raise RuntimeError(f"{source['id']}: missing pin at {destination}; run sync online first")
        destinations.append(destination)
    results = []
    for source, destination in zip(sources, destinations):
        if destination.exists():
            results.append(verify_checkout(destination, source))
            continue
        if dry_run:
            results.append({"id": source["id"], "revision": source["revision"],
                            "path": str(destination), "status": "would-acquire"})
            continue
        locks = _contained(root, "upstream", ".locks")
        staging = _contained(root, "upstream", ".staging")
        locks.mkdir(parents=True, exist_ok=True)
        staging.mkdir(parents=True, exist_ok=True)
        lock = _contained(root, "upstream", ".locks", f"{source['id']}-{source['revision']}")
        try:
            lock.mkdir()
        except FileExistsError as exc:
            raise RuntimeError(f"{source['id']}: another acquisition owns {lock}; do not overwrite it") from exc
        temporary = None
        try:
            temporary = Path(tempfile.mkdtemp(prefix=f"{source['id']}-", dir=staging))
            _git(temporary, "init", "--quiet")
            _git(temporary, "remote", "add", "origin", source["repository_url"])
            _git(temporary, "fetch", "--quiet", "--depth=1", "origin", source["revision"])
            _git(temporary, "checkout", "--quiet", "--detach", source["revision"])
            verify_checkout(temporary, source)
            destination = _contained(root, "upstream", source["id"], source["revision"])
            destination.parent.mkdir(parents=True, exist_ok=True)
            if destination.exists():
                raise RuntimeError(f"{source['id']}: concurrent destination appeared; preserving {destination}")
            temporary.rename(destination)
            temporary = None
            results.append(verify_checkout(destination, source))
        except RuntimeError as exc:
            completed = ", ".join(result["id"] for result in results) or "none"
            raise RuntimeError(f"{exc}\nCompleted sources before failure: {completed}") from exc
        finally:
            if temporary is not None and temporary.exists():
                if temporary.parent.resolve() != staging.resolve() or temporary.is_symlink():
                    raise RuntimeError(f"Refusing unsafe staging cleanup: {temporary}")
                shutil.rmtree(temporary)
            lock.rmdir()
    return results
