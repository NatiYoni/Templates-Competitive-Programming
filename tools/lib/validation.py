"""Independent C++17 checks and content-bound evidence; never acquires sources."""
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shlex
import shutil
import subprocess
import uuid

from . import models

PROFILES = {
    "baseline": ["-std=c++17", "-O2", "-Wall", "-Wextra"],
    "sanitizers": ["-std=c++17", "-O1", "-g", "-Wall", "-Wextra",
                   "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                   "-fno-sanitize-recover=all"],
}
COMPILE_TIMEOUT = 90
RUN_TIMEOUT = 15
RUNNER_FILES = ("tools/toolkit.py", "tools/lib/models.py", "tools/lib/validation.py")
COMPILER_ENV_OVERRIDES = (
    "CPATH", "CPLUS_INCLUDE_PATH", "C_INCLUDE_PATH", "OBJC_INCLUDE_PATH",
    "OBJCPLUS_INCLUDE_PATH", "GCC_EXEC_PREFIX", "COMPILER_PATH", "LIBRARY_PATH",
    "DEPENDENCIES_OUTPUT", "SUNPRO_DEPENDENCIES", "SDKROOT", "LANGUAGE",
)


def canonical(data):
    return json.dumps(data, sort_keys=True, separators=(",", ":"), ensure_ascii=False)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def _output_path(root, relative):
    path = models.safe_path(root, relative)
    current = Path(root)
    for component in path.relative_to(root).parts:
        current /= component
        if current.is_symlink():
            raise ValueError(f"Refusing symlink in validation output path: {current}")
    return path


def run_process(command, root, timeout, env=None, input_text=None):
    """Only argv arrays; a failed/timeout process is an actionable runtime error."""
    try:
        result = subprocess.run(command, cwd=root, capture_output=True, text=True,
                                timeout=timeout, env=env, input=input_text)
    except FileNotFoundError as exc:
        raise RuntimeError(f"Missing executable {command[0]}; install the requested prerequisite") from exc
    except subprocess.TimeoutExpired as exc:
        raise RuntimeError(f"Command timed out after {timeout}s: {shlex.join(map(str, command))}") from exc
    except OSError as exc:
        raise RuntimeError(f"Cannot execute {command[0]}: {exc}") from exc
    if result.returncode:
        details = (result.stderr + "\n" + result.stdout).strip()
        raise RuntimeError(f"Command failed (exit {result.returncode}): "
                           f"{shlex.join(map(str, command))}\n{details}")
    return result


def _compiler_environment():
    env = {key: value for key, value in os.environ.items() if key not in COMPILER_ENV_OVERRIDES}
    env["LC_ALL"] = "C"
    return env


def _compiler_include_roots(root, compiler):
    result = run_process([compiler, "-std=c++17", "-E", "-x", "c++", "-v", "-"],
                         root, COMPILE_TIMEOUT, env=_compiler_environment(), input_text="")
    roots = []
    collecting = False
    for line in result.stderr.splitlines():
        if line.strip() == "#include <...> search starts here:":
            collecting = True
        elif collecting and line.strip() == "End of search list.":
            break
        elif collecting:
            directory = line.strip().removesuffix(" (framework directory)")
            path = Path(directory)
            if not path.is_absolute():
                raise RuntimeError(f"{compiler}: unsupported relative system include root: {directory}")
            if path.is_dir():
                roots.append(str(path.resolve()))
    if not roots:
        raise RuntimeError(f"{compiler}: cannot identify default compiler/system include roots")
    return sorted(set(roots))


def compiler_info(root, compiler="g++"):
    executable = shutil.which(compiler)
    if executable is None:
        raise RuntimeError(f"Missing compiler {compiler}; install GNU g++ with C++17 support")
    result = run_process([executable, "--version"], root, 10, env=_compiler_environment())
    return {"executable": compiler, "version": result.stdout.strip(),
            "language_standard": "c++17", "platform": platform.platform(),
            "machine": platform.machine(),
            "system_include_roots": _compiler_include_roots(root, executable)}


def _runner_hashes(root):
    code_root = Path(__file__).resolve().parents[2]
    hashes = {}
    for relative in RUNNER_FILES:
        actual = code_root / relative
        if actual.is_file():
            hashes[relative] = digest(actual.read_bytes())
        local = models.safe_path(root, relative)
        if local.is_file() and local.resolve() != actual.resolve():
            hashes["checkout:" + relative] = digest(local.read_bytes())
    return hashes


def _include_flags(root, entry, sources):
    flags = ["-I", str(models.safe_path(root, "include")),
             "-I", str(models.safe_path(root, "tests/cpp"))]
    for source_id in entry["integration"]["include_sources"]:
        source = sources[source_id]
        relative = f"upstream/{source_id}/{source['revision']}"
        path = models.safe_path(root, relative)
        if not path.is_dir():
            raise RuntimeError(f"{entry['id']}: missing include source {relative}; run sync --source {source_id}")
        flags.extend(["-I", str(path)])
    return flags


def _declared_paths(entry, sources):
    integration = entry.get("integration")
    paths = []
    if integration:
        paths.extend([integration["example"], *integration["helpers"], *integration["tests"]])
    for reference in entry["references"]:
        source = sources[reference["source"]]
        if source.get("revision"):
            paths.append(f"upstream/{source['id']}/{source['revision']}/{reference['path']}")
    return sorted(set(paths))


def _pins(entry, sources, dependency_paths=()):
    selected = set((entry.get("integration") or {}).get("include_sources", []))
    selected.update(ref["source"] for ref in entry["references"])
    for relative in dependency_paths:
        parts = Path(relative).parts
        if parts[0] == "upstream":
            if len(parts) < 4 or parts[1] not in sources:
                raise RuntimeError(f"{entry['id']}: dependency is not a locked source: {relative}")
            source = sources[parts[1]]
            if not source["acquire"] or parts[2] != source["revision"]:
                raise RuntimeError(f"{entry['id']}: dependency does not match its enabled source pin: {relative}")
            selected.add(parts[1])
    return {sid: {"revision": sources[sid]["revision"],
                  "repository_url": sources[sid]["repository_url"],
                  "license": sources[sid]["license"],
                  "managed_dependencies": sources[sid]["managed_dependencies"]}
            for sid in sorted(selected)}


def _add_dependency(root, entry, dependency, paths, system_roots=()):
    raw = Path(dependency)
    if not raw.is_absolute():
        raw = root / raw
    resolved = raw.resolve()
    lexical = Path(os.path.normpath(str(raw)))
    lexical_local = lexical.is_relative_to(root)
    canonical_local = resolved.is_relative_to(root)
    if not lexical_local:
        if (not canonical_local
                and any(lexical.is_relative_to(directory) for directory in system_roots)
                and any(resolved.is_relative_to(directory) for directory in system_roots)):
            return
        raise RuntimeError(f"{entry['id']}: unsupported external project dependency: {dependency}; "
                           "use declared repository-local dependencies or compiler/system headers")
    if not canonical_local:
        raise RuntimeError(f"{entry['id']}: dependency escapes toolkit root: {dependency}")
    if not lexical.is_file() or lexical.resolve() != resolved:
        raise RuntimeError(f"{entry['id']}: missing or ambiguous dependency path: {dependency}")
    relative = lexical.relative_to(root).as_posix()
    canonical_relative = resolved.relative_to(root).as_posix()
    parts = Path(relative).parts
    if parts[0] == "upstream" and Path(canonical_relative).parts[:3] != parts[:3]:
        raise RuntimeError(f"{entry['id']}: dependency escapes pinned source root: {relative}")
    # Both names are evidence inputs: the local alias must not hide an upstream
    # pin, while retargeting an alias must not retain its previous fingerprint.
    paths.update((relative, canonical_relative))


def _validate_local_dependencies(root, entry, sources, dependencies):
    paths = set()
    for dependency in dependencies:
        _add_dependency(root, entry, dependency, paths)
    _pins(entry, sources, paths)
    return sorted(paths)


def _dependency_aliases(root, paths):
    aliases = {}
    for relative in paths:
        current = root
        for part in Path(relative).parts:
            current /= part
            if current.is_symlink():
                aliases[current.relative_to(root).as_posix()] = os.readlink(current)
    return dict(sorted(aliases.items()))


def _dependency_paths(root, entry, sources, profile, seed, compiler, include_tests=True,
                      system_roots=None):
    root = Path(root).resolve()
    integration = entry.get("integration")
    if not integration:
        raise RuntimeError(f"{entry['id']}: no executable integration; select an implemented entry")
    declared = set(_declared_paths(entry, sources))
    if not include_tests:
        declared.difference_update(integration["tests"])
    paths = set(_validate_local_dependencies(root, entry, sources, declared))
    if system_roots is None:
        system_roots = _compiler_include_roots(root, compiler)
    system_roots = tuple(Path(directory) for directory in system_roots)
    include_flags = _include_flags(root, entry, sources)
    units = [integration["example"], *(integration["tests"] if include_tests else [])]
    for relative in units:
        path = models.safe_path(root, relative)
        if not path.is_file():
            raise RuntimeError(f"{entry['id']}: missing translation unit {relative}; restore its integration")
        command = [compiler, *PROFILES[profile], f"-DTOOLKIT_SEED={seed}",
                   *include_flags, "-M", "-MT", "toolkit-deps", str(path)]
        output = run_process(command, root, COMPILE_TIMEOUT,
                             env=_compiler_environment()).stdout.replace("\\\n", "")
        if not output.startswith("toolkit-deps:"):
            raise RuntimeError(f"{entry['id']}: compiler produced invalid dependency output")
        for dependency in shlex.split(output.split(":", 1)[1]):
            dependency = dependency.replace("$$", "$")
            _add_dependency(root, entry, dependency, paths, system_roots)
    return _validate_local_dependencies(root, entry, sources, paths)


def validate_dependencies(root, entry, sources=None, profile="baseline", include_tests=True):
    """Discover all local headers (including system-marked ones) and verify pins."""
    root = Path(root).resolve()
    if profile not in PROFILES:
        raise ValueError(f"Unknown compiler profile: {profile}")
    sources = models.source_map(sources if sources is not None else models.load_sources(root))
    compiler = compiler_info(root)
    return _dependency_paths(root, entry, sources, profile, 1, compiler["executable"],
                             include_tests=include_tests, system_roots=compiler["system_include_roots"])


def dependency_closure(root, entry, sources=None, profile="baseline"):
    """Repository-relative example/helper/reference closure, without test units.

    Uses the real compiler preprocessor for implemented entries. Source-only
    study material has no configured translation unit and must be rendered
    separately rather than pretending its dependencies were compiler-verified.
    """
    return validate_dependencies(root, entry, sources, profile, include_tests=False)


def fingerprint_entry(root, entry, sources=None, profile="baseline", seed=1,
                      compiler=None, dependency_paths=None):
    root = Path(root).resolve()
    sources = models.source_map(sources if sources is not None else models.load_sources(root))
    if profile not in PROFILES:
        raise ValueError(f"Unknown compiler profile: {profile}")
    compiler = compiler if compiler is not None else compiler_info(root)
    if dependency_paths is None:
        dependency_paths = _dependency_paths(root, entry, sources, profile, seed, compiler["executable"],
                                              system_roots=compiler.get("system_include_roots"))
    dependency_paths = _validate_local_dependencies(root, entry, sources, dependency_paths)
    hashes = {}
    for relative in sorted(set(dependency_paths)):
        path = models.safe_path(root, relative)
        if not path.is_file():
            raise RuntimeError(f"{entry['id']}: missing dependency {relative}; restore it before testing")
        hashes[relative] = digest(path.read_bytes())
    payload = {
        "entry": entry, "source_pins": _pins(entry, sources, dependency_paths), "dependency_hashes": hashes,
        "dependency_aliases": _dependency_aliases(root, dependency_paths),
        "runner_hashes": _runner_hashes(root), "profile": profile, "flags": PROFILES[profile],
        "compiler": compiler, "seed": seed,
        "compile_timeout": COMPILE_TIMEOUT, "run_timeout": RUN_TIMEOUT,
    }
    return {"fingerprint": digest(canonical(payload).encode()), **payload}


def _read_evidence(root, relative):
    path = models.safe_path(root, relative)
    if not path.exists():
        return {"schema_version": 1, "records": []}
    return models.validate_evidence(models.read_json(path))


def load_evidence(root):
    """Overlay every current result, including a failure, over reviewed history."""
    reviewed = _read_evidence(root, "catalog/validation.json")
    current = _read_evidence(root, "build/validation.json")
    records = {(r["entry_id"], r["profile"]): r for r in reviewed["records"]}
    records.update({(r["entry_id"], r["profile"]): r for r in current["records"]})
    return {"schema_version": 1, "records": [records[key] for key in sorted(records)]}


def entry_status(root, entry, sources=None, profile="baseline", evidence=None):
    state = entry["implementation"]
    if state != "implemented":
        return state
    evidence = evidence if evidence is not None else load_evidence(root)
    record = next((r for r in evidence["records"]
                   if r["entry_id"] == entry["id"] and r["profile"] == profile), None)
    if record is None or record["result"] == "not-run":
        return "implemented"
    # A current failure never revives the historical badge, including when
    # dependency extraction itself failed and no complete closure was available.
    if record["result"] == "failed":
        return "failed"
    if record["result"] == "stale":
        return "stale"
    try:
        current = fingerprint_entry(root, entry, sources, profile, record.get("seed", 1))
    except (OSError, RuntimeError):
        return "stale"
    return "tested" if current["fingerprint"] == record["fingerprint"] else "stale"


def display_status(root, entry, profile="baseline"):
    """Public display wrapper for catalog consumers."""
    return entry_status(root, entry, profile=profile)


def _write_json(root, relative, data):
    path = _output_path(root, relative)
    path.parent.mkdir(parents=True, exist_ok=True)
    scratch = _output_path(root, str(path.relative_to(root)) + "." + uuid.uuid4().hex + ".new")
    try:
        with scratch.open("x", encoding="utf-8") as stream:
            json.dump(data, stream, ensure_ascii=False, sort_keys=True, indent=2)
            stream.write("\n")
        scratch.replace(path)
    finally:
        scratch.unlink(missing_ok=True)


def _merge_records(data, records):
    merged = {(r["entry_id"], r["profile"]): r for r in data["records"]}
    merged.update({(r["entry_id"], r["profile"]): r for r in records})
    return {"schema_version": 1, "records": [merged[key] for key in sorted(merged)]}


def _failed_snapshot(root, entry, sources, profile, seed):
    paths = _declared_paths(entry, sources)
    hashes = {p: digest(models.safe_path(root, p).read_bytes())
              for p in paths if models.safe_path(root, p).is_file()}
    payload = {"entry": entry, "source_pins": _pins(entry, sources),
               "dependency_hashes": hashes, "profile": profile, "seed": seed,
               "runner_hashes": _runner_hashes(root), "flags": PROFILES[profile]}
    return {"fingerprint": digest(canonical(payload).encode()), **payload}


def run_tests(root, entry_ids=None, all_core=False, profile="baseline", seed=1, record=False):
    root = Path(root).resolve()
    if profile not in PROFILES:
        raise ValueError(f"Unknown compiler profile: {profile}")
    if type(seed) is not int or not 0 <= seed < 2**32:
        raise ValueError("seed must be an integer in [0, 4294967295]")
    if bool(all_core) == (entry_ids is not None):
        raise ValueError("Select exactly one of all_core or entry_ids")
    sources = models.source_map(models.load_sources(root))
    catalog = models.load_catalog(root)
    by_id = {e["id"]: e for e in catalog["entries"]}
    if all_core:
        selected = list(models.load_profile(root, "notebook/profiles/core.json")["entries"])
    else:
        if (not isinstance(entry_ids, list) or not entry_ids or
                any(not isinstance(item, str) for item in entry_ids) or len(entry_ids) != len(set(entry_ids))):
            raise ValueError("Select at least one unique entry ID")
        selected = entry_ids
    unknown = set(selected) - set(by_id)
    if unknown:
        raise ValueError("Unknown entries: " + ", ".join(sorted(unknown)))
    if not selected:
        raise ValueError("No core entries selected")
    # Validate every destination and existing report before any compilation/write.
    build = _output_path(root, f"build/tests/{profile}")
    _output_path(root, "build/validation.json")
    for entry_id in selected:
        integration = by_id[entry_id].get("integration")
        for index in range(1 + len(integration["tests"]) if integration else 0):
            _output_path(root, f"build/tests/{profile}/{entry_id}-{index}")
    current = _read_evidence(root, "build/validation.json")
    reviewed = _read_evidence(root, "catalog/validation.json")
    if record:
        _output_path(root, "catalog/validation.json")
    records = []
    for entry_id in selected:
        item = by_id[entry_id]
        snapshot = _failed_snapshot(root, item, sources, profile, seed)
        result = {
            "entry_id": entry_id, "profile": profile, "result": "failed", "seed": seed,
            "case_count": 0, "checks": [], "test_results": [], "failure": "",
            "compile_timeout": COMPILE_TIMEOUT, "run_timeout": RUN_TIMEOUT,
            **snapshot,
        }
        try:
            if not item.get("integration"):
                raise RuntimeError(f"{entry_id}: missing integration; this is {item['implementation']}")
            if item["copy_policy"]["status"] != "permitted":
                raise RuntimeError(f"{entry_id}: copying/integration is blocked: {item['reason']}")
            compiler = compiler_info(root)
            snapshot = fingerprint_entry(root, item, sources, profile, seed, compiler)
            result.update(snapshot)
            build.mkdir(parents=True, exist_ok=True)
            includes = _include_flags(root, item, sources)
            integration = item["integration"]
            units = [("example", integration["example"])] + [("oracle", p) for p in integration["tests"]]
            for index, (kind, relative) in enumerate(units):
                binary = _output_path(root, f"build/tests/{profile}/{entry_id}-{index}")
                command = [compiler["executable"], *PROFILES[profile], f"-DTOOLKIT_SEED={seed}",
                           *includes, str(models.safe_path(root, relative)), "-o", str(binary)]
                env = _compiler_environment()
                env["TMPDIR"] = str(build)
                compiled = run_process(command, root, COMPILE_TIMEOUT, env=env)
                if profile == "sanitizers":
                    env["ASAN_OPTIONS"] = "detect_leaks=1:halt_on_error=1"
                    env["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
                executed = run_process([str(binary)], root, RUN_TIMEOUT, env=env)
                check = {"path": relative, "kind": kind, "result": "passed",
                         "stdout": executed.stdout, "stderr": executed.stderr,
                         "compiler_diagnostics": compiled.stderr, "seed": seed, "case_count": 0}
                if kind == "oracle":
                    matches = re.findall(r"^OK cases=(\d+) seed=(\d+)\s*$", executed.stdout, re.MULTILINE)
                    if len(matches) != 1 or int(matches[0][0]) < 1 or int(matches[0][1]) != seed:
                        raise RuntimeError(f"{entry_id}: {relative} must print exactly one "
                                           f"'OK cases=N seed={seed}' line with positive cases; got {executed.stdout!r}")
                    check["case_count"] = int(matches[0][0])
                    result["case_count"] += check["case_count"]
                result["test_results"].append(check)
            after = fingerprint_entry(root, item, sources, profile, seed, compiler)
            if after["fingerprint"] != snapshot["fingerprint"]:
                raise RuntimeError(f"{entry_id}: dependencies changed during validation; rerun on stable inputs")
            result["checks"] = ["standalone-compilation", "example-execution", "edge-cases", "local-oracle"]
            if profile == "sanitizers":
                result["checks"].append("sanitizers")
            result["result"] = "passed"
        except (RuntimeError, OSError) as exc:
            result["failure"] = f"{entry_id}: {exc}"
        records.append(result)
        # Persist each completed result so a later interrupted entry cannot hide it.
        current = _merge_records(current, [result])
        _write_json(root, "build/validation.json", current)
        if record:
            reviewed = _merge_records(reviewed, [result])
            _write_json(root, "catalog/validation.json", reviewed)
    return {"ok": all(r["result"] == "passed" for r in records), "records": records,
            "evidence": reviewed if record else current}
