"""Versioned metadata validation and confined toolkit paths (stdlib only)."""
import json
import html
from pathlib import Path, PurePosixPath
import re

ID = re.compile(r"[a-z0-9]+(?:-[a-z0-9]+)*\Z")
SHA = re.compile(r"[0-9a-f]{40}\Z")
HASH = re.compile(r"[0-9a-f]{64}\Z")
CONTRACT_FIELDS = (
    "purpose", "input", "output", "preconditions", "indexing", "time", "space",
    "numeric_limits", "mutation", "global_state", "recursion", "limitations",
)


def _require(condition, message):
    if not condition:
        raise ValueError(message)


def _text(value, label, nonempty=True):
    _require(isinstance(value, str) and (not nonempty or bool(value.strip())),
             f"{label}: expected {'nonempty ' if nonempty else ''}string")


def _strings(value, label, unique=False):
    _require(isinstance(value, list), f"{label}: expected array")
    for item in value:
        _text(item, label)
    if unique:
        _require(len(value) == len(set(value)), f"{label}: duplicate values")


def _identifier(value, label):
    _require(isinstance(value, str) and ID.fullmatch(value), f"{label}: invalid kebab-case ID")


def relative_path(value):
    _text(value, "path")
    _require("\\" not in value and not any(ord(c) < 32 or ord(c) == 127 for c in value),
             f"Invalid path: {value!r}")
    path = PurePosixPath(value)
    _require(not path.is_absolute() and all(p not in ("", ".", "..") for p in value.split("/"))
             and not re.match(r"^[A-Za-z]:", value), f"Path must be normalized and relative: {value!r}")
    return path


def safe_path(root, relative, must_exist=False):
    root = Path(root).resolve()
    path = root.joinpath(*relative_path(relative).parts)
    _require(path.resolve().is_relative_to(root), f"Path escapes toolkit root: {relative}")
    if must_exist:
        _require(path.exists(), f"Missing declared path: {relative}; restore it or correct the metadata")
    return path


def markdown_prose(text):
    """Ignore fenced source examples when inspecting Markdown headings/links."""
    lines, fence = [], None
    for line in text.splitlines():
        marker = re.match(r"^ {0,3}(`{3,}|~{3,})(.*)$", line)
        if fence is not None:
            if (marker and marker.group(1)[0] == fence[0]
                    and len(marker.group(1)) >= len(fence) and not marker.group(2).strip()):
                fence = None
            lines.append("")
        elif marker:
            fence = marker.group(1)
            lines.append("")
        else:
            lines.append(line)
    return re.sub(r"<!--.*?(?:-->|$)", lambda match: "\n" * match.group().count("\n"),
                  "\n".join(lines), flags=re.DOTALL)


def markdown_heading_anchors(text):
    anchors = set()
    previous = ""
    for line in markdown_prose(text).splitlines():
        match = re.match(r"^ {0,3}#{1,6}\s+(.+?)\s*#*\s*$", line)
        heading = match.group(1) if match else None
        if heading is None and previous.strip() and re.fullmatch(r" {0,3}(?:=+|-+)\s*", line):
            heading = previous.strip()
        if heading is not None:
            heading = re.sub(r"<[^>]*>", "", heading)
            heading = re.sub(r"!?\[([^]]*)\]\([^)]*\)", r"\1", heading)
            slug = re.sub(r"[^\w -]", "", html.unescape(heading).casefold()).replace(" ", "-")
            candidate, suffix = slug, 0
            while candidate in anchors:
                suffix += 1
                candidate = f"{slug}-{suffix}"
            anchors.add(candidate)
        previous = line if heading is None else ""
    return anchors


def local_reference_path(root, reference, check_paths=False):
    """Original prose cannot borrow a source identity via a filesystem alias."""
    relative = relative_path(reference.get("path"))
    _require(relative.parts[:2] == ("docs", "techniques") and len(relative.parts) >= 3
             and relative.suffix == ".md",
             "Local reference must be a docs/techniques/*.md article")
    if root is None:
        return None
    root = Path(root).resolve()
    try:
        path = safe_path(root, relative.as_posix())
        try:
            canonical = path.resolve(strict=True)
        except FileNotFoundError:
            canonical = path.resolve()
    except (OSError, RuntimeError) as exc:
        raise ValueError(f"Cannot resolve original educational reference {relative}: {exc}") from exc
    allowed = root / "docs/techniques"
    _require(canonical.is_relative_to(allowed),
             f"Local reference escapes docs/techniques: {relative}")
    _require(canonical.suffix == ".md", f"Local reference must resolve to Markdown: {relative}")
    if check_paths:
        _require(path.is_file(), f"Missing original educational reference: {relative}; restore the document")
        try:
            text = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as exc:
            raise ValueError(f"Cannot read original educational reference {relative}: {exc}") from exc
        if "fragment" in reference:
            _require(reference["fragment"] in markdown_heading_anchors(text),
                     f"Missing original reference heading: {relative}#{reference['fragment']}")
    return path


def _object_pairs(pairs):
    result = {}
    for key, value in pairs:
        _require(key not in result, f"Duplicate JSON key: {key}")
        result[key] = value
    return result


def read_json(path):
    try:
        with Path(path).open(encoding="utf-8") as stream:
            return json.load(stream, object_pairs_hook=_object_pairs)
    except json.JSONDecodeError as exc:
        raise ValueError(f"{path}: invalid JSON: {exc}") from exc
    except UnicodeError as exc:
        raise ValueError(f"{path}: expected UTF-8 JSON: {exc}") from exc


def _document(data, collection):
    _require(isinstance(data, dict), "Metadata root must be an object")
    _require(type(data.get("schema_version")) is int and data["schema_version"] == 1,
             "Unsupported schema_version; expected integer 1")
    _require(isinstance(data.get(collection), list), f"{collection}: expected array")
    return data[collection]


def _unique_objects(items, label, id_key="id"):
    seen = set()
    for item in items:
        _require(isinstance(item, dict), f"{label}: expected object")
        item_id = item.get(id_key)
        _identifier(item_id, label)
        _require(item_id not in seen, f"Duplicate {label} ID: {item_id}")
        seen.add(item_id)


def validate_sources(data, root=None):
    sources = _document(data, "sources")
    _unique_objects(sources, "source")
    by_id = {s["id"]: s for s in sources}
    for source in sources:
        label = source["id"]
        _text(source.get("name"), f"{label}.name")
        _require(type(source.get("acquire")) is bool, f"{label}.acquire: expected boolean")
        _require(source.get("kind") in ("reusable-library", "educational-articles",
                 "mixed-notebook-archive", "external-study-reference"), f"{label}: invalid source kind")
        for field in ("attribution", "availability", "license"):
            _require(isinstance(source.get(field), dict), f"{label}.{field}: expected object")
        _require(source["attribution"].get("status") in ("verified", "unverified", "not-applicable"),
                 f"{label}: invalid attribution status")
        _text(source["license"].get("declared"), f"{label}.license.declared")
        url, revision = source.get("repository_url"), source.get("revision")
        if source["acquire"] or url is not None:
            _require(isinstance(url, str) and re.fullmatch(
                r"https://github\.com/[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+", url)
                and all(p not in (".", "..") for p in url.split("/")[-2:]),
                f"{label}: expected canonical HTTPS GitHub repository URL")
        if source["acquire"] or revision is not None:
            _require(isinstance(revision, str) and SHA.fullmatch(revision),
                     f"{label}: expected full lowercase commit SHA")
        if source["acquire"]:
            _require(source["attribution"]["status"] == "verified",
                     f"{label}: enabled acquisition requires verified attribution")
        _strings(source.get("portability_notes"), f"{label}.portability_notes")
        _strings(source.get("managed_dependencies"), f"{label}.managed_dependencies", unique=True)
        for dep in source["managed_dependencies"]:
            _require(dep in by_id and dep != label, f"{label}: unknown/self managed dependency {dep}")
        _require(isinstance(source.get("submodules"), list), f"{label}.submodules: expected array")
        seen = set()
        for submodule in source["submodules"]:
            _require(isinstance(submodule, dict), f"{label}.submodule: expected object")
            relative_path(submodule.get("path"))
            _require(submodule["path"] not in seen, f"{label}: duplicate submodule path")
            seen.add(submodule["path"])
            _require(isinstance(submodule.get("revision"), str) and SHA.fullmatch(submodule["revision"]),
                     f"{label}: malformed submodule pin")
        if root is not None and revision is not None:
            safe_path(root, f"upstream/{label}/{revision}")
    return data


def source_map(sources):
    if isinstance(sources, dict) and "sources" in sources:
        return {s["id"]: s for s in sources["sources"]}
    if isinstance(sources, list):
        return {s["id"]: s for s in sources}
    return sources


def validate_catalog(data, sources, root=None, check_paths=False):
    entries = _document(data, "entries")
    _unique_objects(entries, "entry")
    sources = source_map(sources)
    for entry in entries:
        label = entry["id"]
        for field in ("name", "category", "subcategory"):
            _text(entry.get(field), f"{label}.{field}", nonempty=field != "subcategory")
        for field in ("aliases", "tags"):
            _strings(entry.get(field), f"{label}.{field}", unique=True)
        _require(entry.get("priority") in ("core", "advanced"), f"{label}: invalid priority")
        state = entry.get("implementation")
        _require(state in ("missing", "reference-only", "implemented"), f"{label}: invalid implementation")
        _text(entry.get("reason"), f"{label}.reason", nonempty=state == "missing")
        _require(isinstance(entry.get("contract"), dict), f"{label}.contract: expected object")
        for field in CONTRACT_FIELDS:
            _text(entry["contract"].get(field), f"{label}.contract.{field}", nonempty=state == "implemented")
        policy = entry.get("copy_policy")
        _require(isinstance(policy, dict) and policy.get("status") in ("permitted", "blocked"),
                 f"{label}: invalid copy_policy")
        _strings(policy.get("notices"), f"{label}.copy_policy.notices")
        if policy["status"] == "blocked":
            _text(entry["reason"], f"{label}: blocked copying requires a reason")
        refs = entry.get("references")
        _require(isinstance(refs, list), f"{label}.references: expected array")
        local_refs = entry.get("local_references", [])
        _require(isinstance(local_refs, list), f"{label}.local_references: expected array")
        if state == "reference-only":
            _require(bool(refs or local_refs), f"{label}: reference-only entry needs references or local_references")
        seen_local = set()
        for ref in local_refs:
            _require(isinstance(ref, dict), f"{label}: local reference must be an object")
            _require(set(ref) <= {"path", "fragment", "attribution", "provenance", "kind"},
                     f"{label}: unsupported local reference fields; original prose has no upstream pin or license")
            _text(ref.get("attribution"), f"{label}.local_reference.attribution")
            _require(ref.get("provenance") == "repository-original",
                     f"{label}: local reference requires repository-original provenance")
            _require(ref.get("kind") == "article", f"{label}: local reference must be an article")
            if "fragment" in ref:
                _require(isinstance(ref["fragment"], str) and re.fullmatch(r"[\w-]+", ref["fragment"]),
                         f"{label}: local reference fragment must be a heading slug")
            local_reference_path(root, ref, check_paths)
            identity = (ref["path"], ref.get("fragment"))
            _require(identity not in seen_local, f"{label}: duplicate local reference")
            seen_local.add(identity)
        for ref in refs:
            _require(isinstance(ref, dict), f"{label}: reference must be an object")
            source_id = ref.get("source")
            _require(isinstance(source_id, str) and source_id in sources,
                     f"{label}: unknown reference source {source_id!r}")
            relative_path(ref.get("path"))
            _require(ref.get("kind") in ("code", "article"), f"{label}: invalid reference kind")
            _text(ref.get("license"), f"{label}.reference.license")
            if policy["status"] == "permitted":
                _require(ref["license"].upper() not in ("NOASSERTION", "UNKNOWN", "UNLICENSED"),
                         f"{label}: unresolved reference license cannot permit copying")
            source = sources[source_id]
            if root is not None and source.get("revision"):
                base = f"upstream/{source_id}/{source['revision']}"
                source_root = safe_path(root, base)
                path = safe_path(source_root, ref["path"])
                if check_paths and source_root.exists():
                    _require(path.is_file(), f"{label}: missing referenced file {path}")
        integration = entry.get("integration")
        if state == "implemented":
            _require(isinstance(integration, dict), f"{label}: implemented entry needs integration")
        else:
            _require(integration is None, f"{label}: only implemented entries may have integration")
        if integration is not None:
            relative_path(integration.get("example"))
            _require(integration["example"].startswith("examples/") and integration["example"].endswith(".cpp"),
                     f"{label}: example must be an examples/*.cpp file")
            for field in ("helpers", "include_sources", "tests"):
                _strings(integration.get(field), f"{label}.integration.{field}", unique=True)
            _require(bool(integration["tests"]), f"{label}: integration requires local tests")
            for path in integration["tests"]:
                _require(path.startswith("tests/cpp/") and path.endswith(".cpp"),
                         f"{label}: test must be a tests/cpp/*.cpp file")
            for source_id in integration["include_sources"]:
                _require(source_id in sources and sources[source_id]["acquire"],
                         f"{label}: unknown or disabled include source {source_id}")
            for path in [integration["example"], *integration["helpers"], *integration["tests"]]:
                relative_path(path)
                if root is not None:
                    resolved = safe_path(root, path)
                    if check_paths:
                        _require(resolved.is_file(), f"{label}: missing declared file {path}")
    return data


def validate_profile(data, catalog=None):
    entries = _document(data, "entries")
    _text(data.get("name"), "profile.name")
    _text(data.get("description", ""), "profile.description", nonempty=False)
    _strings(entries, "profile.entries", unique=True)
    _require(bool(entries), "profile.entries: select at least one entry")
    for item in entries:
        _identifier(item, "profile entry")
    _require(type(data.get("include_study_references", False)) is bool,
             "profile.include_study_references: expected boolean")
    _require(isinstance(data.get("print_options", {}), dict), "profile.print_options: expected object")
    if catalog is not None:
        known = {e["id"] for e in catalog["entries"]}
        _require(set(entries) <= known, f"Unknown profile entries: {', '.join(sorted(set(entries) - known))}")
    return data


def validate_evidence(data):
    records = _document(data, "records")
    seen = set()
    for record in records:
        _require(isinstance(record, dict), "Evidence record must be an object")
        _identifier(record.get("entry_id"), "evidence entry_id")
        _require(record.get("profile") in ("baseline", "sanitizers"), "Unknown evidence profile")
        key = record["entry_id"], record["profile"]
        _require(key not in seen, f"Duplicate evidence record: {key}")
        seen.add(key)
        _require(record.get("result") in ("not-run", "passed", "failed", "stale"), "Invalid evidence result")
        fingerprint = record.get("fingerprint")
        _require(isinstance(fingerprint, str) and HASH.fullmatch(fingerprint), "Malformed evidence fingerprint")
        hashes = record.get("dependency_hashes")
        _require(isinstance(hashes, dict), "Evidence dependency_hashes must be an object")
        for path, digest in hashes.items():
            relative_path(path)
            _require(isinstance(digest, str) and HASH.fullmatch(digest), f"Malformed dependency hash: {path}")
    return data


def load_sources(root):
    return validate_sources(read_json(safe_path(root, "sources/lock.json", True)), root)


def load_catalog(root):
    return validate_catalog(read_json(safe_path(root, "catalog/algorithms.json", True)),
                            load_sources(root), root=root)


def load_profile(root, path):
    return validate_profile(read_json(safe_path(root, str(path), True)), load_catalog(root))
