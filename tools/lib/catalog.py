"""Offline catalog queries; implementation and evidence are deliberately separate."""
from collections import Counter
import hashlib
from pathlib import Path

from . import models


def load_catalog(root):
    return models.load_catalog(root)


def source_root(root, source):
    if not source.get("revision"):
        return None
    return models.safe_path(root, f"upstream/{source['id']}/{source['revision']}")


def reference_path(root, source, reference):
    base = source_root(root, source)
    if base is None:
        return None
    path = models.safe_path(base, reference["path"])
    if base.exists() and not path.is_file():
        raise ValueError(f"{source['id']}: missing referenced file {reference['path']}; restore the pinned checkout")
    return path


def _evidence_status(root, entry):
    from .validation import entry_status
    return entry_status(root, entry, profile="baseline")


def search_text(entry):
    """The CLI and embedded browser data use the same authored search fields."""
    values = [entry["id"], entry["name"], entry["reason"], entry["category"], entry["subcategory"]]
    values.extend(entry["aliases"])
    values.extend(entry["tags"])
    values.extend(entry["contract"].values())
    values.extend(entry["copy_policy"]["notices"])
    for reference in entry.get("local_references", []):
        values.extend(reference.values())
    for location in entry.get("locations", []):
        if location.get("provenance") == "repository-original" and location["content_hash"]:
            values.append(location["content_hash"])
    return "\n".join(values).casefold()


def catalog_entries(root):
    root = Path(root).resolve()
    document = load_catalog(root)
    sources = models.source_map(models.load_sources(root))
    result = []
    for original in document["entries"]:
        entry = dict(original)
        locations = []
        for reference in entry["references"]:
            source = sources[reference["source"]]
            path = reference_path(root, source, reference)
            available = bool(path and path.is_file())
            locations.append({
                **reference, "available": available,
                "local_path": path.relative_to(root).as_posix() if available else None,
                "revision": source.get("revision"),
                "author": source["attribution"].get("author", source["name"]),
            })
        for reference in entry.get("local_references", []):
            path = models.local_reference_path(root, reference)
            available = path.is_file()
            if available:
                models.local_reference_path(root, reference, check_paths=True)
            locations.append({
                **reference, "available": available,
                "local_path": reference["path"] if available else None,
                "author": reference["attribution"],
                "content_hash": hashlib.sha256(path.read_bytes()).hexdigest() if available else None,
            })
        local_missing = False
        if entry["integration"]:
            for relative in [entry["integration"]["example"], *entry["integration"]["helpers"], *entry["integration"]["tests"]]:
                if not models.safe_path(root, relative).is_file():
                    local_missing = True
        entry["availability"] = (
            "unavailable" if local_missing or any(not r["available"] for r in locations)
            else "available" if locations or entry["integration"] else "not-applicable"
        )
        entry["status"] = entry["implementation"]
        if entry["implementation"] == "implemented":
            entry["status"] = _evidence_status(root, original)
            if entry["availability"] == "unavailable" and entry["status"] == "tested":
                entry["status"] = "stale"
        entry["locations"] = locations
        entry["search_text"] = search_text(entry)
        result.append(entry)
    return sorted(result, key=lambda entry: (entry["name"].casefold(), entry["id"]))


def search(root, query="", category=None, priority=None, status=None):
    valid = {"missing", "reference-only", "implemented", "tested", "failed", "stale"}
    if status is not None and status not in valid:
        raise ValueError(f"Unknown status: {status}")
    if priority is not None and priority not in {"core", "advanced"}:
        raise ValueError(f"Unknown priority: {priority}")
    query = query.casefold()
    return [
        entry for entry in catalog_entries(root)
        if query in entry["search_text"]
        and (category is None or category.casefold() == entry["category"].casefold())
        and (priority is None or priority == entry["priority"])
        and (status is None or status == entry["status"])
    ]


def counts(entries):
    return {
        "total": len(entries),
        "status": dict(sorted(Counter(e["status"] for e in entries).items())),
        "category": dict(sorted(Counter(e["category"] for e in entries).items())),
        "source": dict(sorted(Counter(
            source for entry in entries for source in {ref["source"] for ref in entry["references"]}
        ).items())),
    }
