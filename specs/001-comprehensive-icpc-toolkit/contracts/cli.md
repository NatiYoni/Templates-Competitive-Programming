# CLI Contract

Proposed executable: `python3 tools/toolkit.py`. Commands below are design
contracts, not available toolkit commands during this planning change.

## Common Behavior

- Run relative to the toolkit root, with an explicit validated `--root` option
  for a separate clean checkout.
- Exit 0 only on complete requested success; exit 2 for invalid input/schema;
  exit 1 for acquisition, missing prerequisites, compilation, or test failures.
- Diagnostics go to stderr and identify source/entry/path and remedy.
- `--json` yields a stable machine-readable result; no progress chatter on
  stdout in that mode.
- No implicit network calls outside `sync` without `--offline`.
- Validate paths and inputs before side effects. Never execute untrusted shell
  strings, upstream build scripts, or recursive submodules.

## Commands

| Command | Contract |
|---------|----------|
| `doctor [--json]` | Read-only Git/Python/compiler availability/version/profile diagnostics; fail when required baseline tools are missing. |
| `sync (--all \| --source ID...) [--offline] [--dry-run] [--json]` | Acquire only enabled approved locked sources. `--all` and explicit selections are mutually exclusive. Reject disabled/unknown selections. Offline verifies existing pins and never fetches. Dry-run writes nothing. |
| `validate [--sources] [--catalog] [--links] [--json]` | Check requested lock/source/catalog/dependency/permission/link invariants; default validates lock/catalog structure without fetching. Broken declared paths are errors. |
| `search QUERY [--category NAME] [--priority core\|advanced] [--status STATUS] [--json]` | Offline case-insensitive name/alias/tag/note search. Read only, deterministic ordering, status computed against evidence. |
| `test (--all-core \| --entry ID...) [--profile baseline\|sanitizers] [--seed N] [--record] [--json]` | Build and run selected examples/tests offline. Timeouts and oracle mismatches fail. Default report is ignored build output; `--record` explicitly updates reviewed catalog evidence. Missing optional profile capabilities fail that requested profile clearly. |
| `index [--output-dir build] [--json]` | Generate HTML/Markdown indexes from validated metadata, source availability, and evidence. Offline search data/assets are local. Stable output with no timestamps. |
| `notebook --profile PATH [--output-dir build/notebook] [--json]` | Generate ordered HTML/Markdown selection and dependency/notices appendices. Fail for invalid, unavailable, untested, or license-blocked ready-to-use entries. |

`STATUS` accepts `missing`, `reference-only`, `implemented`, `tested`, `failed`,
or `stale`; the last three are derived evidence displays rather than
implementation enum values. Search with no matches returns an empty successful
result, not an exception or fabricated result.

## Safe Synchronization Invariants

The source lock is input, never automatically updated. An existing wrong/
modified destination is an error, never a reset request. Validate the complete
request before acquisition starts; if a later network failure occurs, report
which sources completed and which failed, while returning nonzero overall.
Atomicity is per source, not a promise of all-or-nothing network acquisition.
Cleanup is limited to the invocation's own staging directory. A concurrent
destination creation causes an explicit conflict or a verified identical
checkout result; it must not overwrite the other process's work.

## C++ Example Contract

Each `examples/<entry-id>.cpp` is independently compilable under its declared
profile with include roots supplied by the runner. It is a usage demonstration,
not a universal judge I/O format. It documents inputs, output, preconditions,
and required upstream files. It must not rely on undisclosed global headers,
build artifacts, external libraries, or downloaded runtime dependencies.

## Generated Document Contract

HTML opens via `file://` and has no required remote resources. Embedded search
data and source previews are escaped against HTML/script terminators. Paths
with spaces and reserved characters are URL-encoded; links resolve relative
to their document. Internal/local anchors and dependency links are checked.
External provenance links are labeled external and are not falsely claimed to
work without a network.
