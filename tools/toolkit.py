#!/usr/bin/env python3
"""Offline-first toolkit CLI. Only an explicit online sync may contact sources."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys

from lib import models, validation

ROOT = Path(__file__).resolve().parents[1]
STATUSES = ("missing", "reference-only", "implemented", "tested", "failed", "stale")


class Parser(argparse.ArgumentParser):
    def error(self, message):
        raise ValueError(message)


def parser():
    common = Parser(add_help=False)
    common.add_argument("--root", default=argparse.SUPPRESS, help="Toolkit checkout root")
    common.add_argument("--json", action="store_true", default=argparse.SUPPRESS,
                        help="Emit a stable JSON result")
    result = Parser(description=__doc__, parents=[common])
    commands = result.add_subparsers(dest="command", required=True, parser_class=Parser)
    commands.add_parser("doctor", parents=[common], help="Read-only baseline prerequisite diagnostics")
    sync = commands.add_parser("sync", parents=[common], help="Explicitly acquire/verify pinned sources")
    choice = sync.add_mutually_exclusive_group(required=True)
    choice.add_argument("--all", action="store_true", help="Select all enabled sources")
    choice.add_argument("--source", nargs="+", action="extend", metavar="ID")
    sync.add_argument("--offline", action="store_true")
    sync.add_argument("--dry-run", action="store_true")
    validate = commands.add_parser("validate", parents=[common], help="Validate selected local invariants")
    for name in ("sources", "catalog", "links"):
        validate.add_argument("--" + name, action="store_true")
    search = commands.add_parser("search", parents=[common], help="Search offline catalog metadata")
    search.add_argument("query", metavar="QUERY")
    search.add_argument("--category")
    search.add_argument("--priority", choices=("core", "advanced"))
    search.add_argument("--status", choices=STATUSES)
    test = commands.add_parser("test", parents=[common], help="Compile/run local examples and oracle tests")
    choice = test.add_mutually_exclusive_group(required=True)
    choice.add_argument("--all-core", action="store_true",
                        help="Select entries from notebook/profiles/core.json, not all core-priority study topics")
    choice.add_argument("--entry", nargs="+", action="extend", metavar="ID")
    choice.add_argument("--selection-profile", metavar="PATH",
                        help="Select the exact ordered entries in a notebook profile inside the toolkit root")
    test.add_argument("--profile", choices=tuple(validation.PROFILES), default="baseline")
    test.add_argument("--seed", type=int, default=1)
    test.add_argument("--record", action="store_true", help="Explicitly update reviewed catalog evidence")
    index = commands.add_parser("index", parents=[common], help="Generate offline HTML and Markdown indexes")
    index.add_argument("--output-dir", default="build")
    notebook = commands.add_parser("notebook", parents=[common], help="Generate an ordered offline notebook")
    notebook.add_argument("--profile", required=True, metavar="PATH")
    notebook.add_argument("--output-dir", default="build/notebook")
    return result


def doctor(root):
    tools = [{"name": "python", "available": sys.version_info >= (3, 10),
              "version": sys.version.split()[0], "required": "Python 3.10+"}]
    for name in ("git", "g++"):
        executable = shutil.which(name)
        item = {"name": name, "available": executable is not None, "version": ""}
        if executable is not None:
            try:
                item["version"] = validation.run_process([executable, "--version"], root, 10).stdout.strip()
            except RuntimeError as exc:
                item.update(available=False, error=str(exc))
        else:
            item["error"] = f"Install {name} and make it available on PATH"
        tools.append(item)
    return {"ok": all(t["available"] for t in tools), "tools": tools,
            "profiles": validation.PROFILES,
            "notes": ["Core target: GNU C++17/libstdc++; exact ETCPC compiler unverified.",
                      "Compact snippets may include bits/stdc++.h; ISO-only toolchains are not the baseline.",
                      "Sanitizers are optional and fail explicitly when unavailable; no fallback."]}


def validate(root, args):
    sources = models.load_sources(root)
    document = models.load_catalog(root)
    checks = [{"check": "source-lock-schema", "ok": True, "count": len(sources["sources"])},
              {"check": "catalog-schema", "ok": True, "count": len(document["entries"])}]
    if args.sources:
        from lib.acquire import acquire_sources
        acquired = acquire_sources(root, offline=True)
        checks.append({"check": "source-pins-offline", "ok": True, "sources": acquired})
    if args.catalog:
        models.validate_catalog(document, sources, root=root, check_paths=True)
        by_id = models.source_map(sources)
        for item in document["entries"]:
            if item["integration"] is not None:
                validation.validate_dependencies(root, item, by_id)
        checks.append({"check": "catalog-paths-and-dependencies", "ok": True})
    # Historical reports are metadata too; malformed evidence must not be hidden
    # behind an apparently successful default validate command.
    evidence = validation.load_evidence(root)
    known = {item["id"] for item in document["entries"]}
    unknown = {r["entry_id"] for r in evidence["records"]} - known
    if unknown:
        raise ValueError("Evidence references unknown entries: " + ", ".join(sorted(unknown)))
    checks.append({"check": "evidence-schema", "ok": True, "count": len(evidence["records"])})
    if args.links:
        from lib.render import validate_links
        errors = validate_links(root)
        if errors:
            raise RuntimeError("Broken local links: " + "; ".join(errors))
        checks.append({"check": "offline-links", "ok": True})
    return {"ok": True, "checks": checks}


def dispatch(root, args):
    if args.command == "doctor":
        return doctor(root)
    if args.command == "sync":
        from lib.acquire import acquire_sources
        models.load_sources(root)
        return {"ok": True, "sources": acquire_sources(root, None if args.all else args.source,
                                                       offline=args.offline, dry_run=args.dry_run)}
    if args.command == "validate":
        return validate(root, args)
    if args.command == "search":
        from lib.catalog import search
        entries = search(root, args.query, category=args.category, priority=args.priority, status=args.status)
        return {"ok": True, "entries": entries, "count": len(entries)}
    if args.command == "test":
        entry_ids = (models.load_profile(root, args.selection_profile)["entries"]
                     if args.selection_profile is not None else args.entry)
        return validation.run_tests(root, entry_ids=entry_ids, all_core=args.all_core,
                                    profile=args.profile, seed=args.seed, record=args.record)
    if args.command == "index":
        from lib.render import generate_index
        result = generate_index(root, args.output_dir)
        return {"ok": True, **result}
    if args.command == "notebook":
        models.load_profile(root, args.profile)
        from lib.render import generate_notebook
        result = generate_notebook(root, args.profile, args.output_dir)
        return {"ok": True, **result}
    raise ValueError(f"Unknown command: {args.command}")


def _messages(result):
    messages = []
    for tool in result.get("tools", []):
        if not tool["available"]:
            messages.append({"level": "error", "message": tool.get("error", f"Required {tool['name']} unavailable")})
    for record in result.get("records", []):
        if record["result"] != "passed":
            messages.append({"level": "error", "entry": record["entry_id"], "message": record["failure"]})
    return messages


def _plain(result):
    command = result["command"]
    if command == "search":
        for entry in result.get("entries", []):
            print(f"{entry['id']}\t{entry['status']}\t{entry['name']}")
        if not result.get("entries"):
            print("No matches.")
    elif command == "test":
        for record in result.get("records", []):
            print(f"{record['entry_id']}: {record['result']} ({record['profile']}, "
                  f"cases={record['case_count']}, seed={record['seed']})")
    elif command == "doctor":
        for tool in result.get("tools", []):
            print(f"{tool['name']}: {tool['version'].splitlines()[0] if tool['version'] else 'unavailable'}")
        for note in result.get("notes", []):
            print(note)
    elif command == "sync":
        for source in result.get("sources", []):
            print(f"{source['id']}: {source['status']} {source['revision']}")
    elif result["ok"]:
        print(f"{command}: OK")
        for key in ("html", "markdown"):
            if key in result:
                print(result[key])
    for message in result["messages"]:
        print(message["message"], file=sys.stderr)


def main(argv=None):
    argv = sys.argv[1:] if argv is None else list(argv)
    json_mode = "--json" in argv
    command = next((arg for arg in argv if arg in (
        "doctor", "sync", "validate", "search", "test", "index", "notebook")), None)
    try:
        args = parser().parse_args(argv)
        json_mode = getattr(args, "json", False)
        command = args.command
        root = Path(getattr(args, "root", ROOT)).expanduser().resolve()
        if not root.is_dir():
            raise ValueError(f"Toolkit root is not a directory: {root}")
        result = dispatch(root, args)
        result["command"] = command
        result["messages"] = _messages(result)
        code = 0 if result["ok"] else 1
    except ValueError as exc:
        result = {"ok": False, "command": command, "messages": [{"level": "error", "message": str(exc)}]}
        code = 2
    except (OSError, RuntimeError) as exc:
        result = {"ok": False, "command": command, "messages": [{"level": "error", "message": str(exc)}]}
        code = 1
    if json_mode:
        print(json.dumps(result, ensure_ascii=False, sort_keys=True))
        for message in result["messages"]:
            print(message["message"], file=sys.stderr)
    else:
        _plain(result)
    return code


if __name__ == "__main__":
    sys.exit(main())
