# Usage and maintenance

Run commands from this repository, or pass `--root PATH` to target another
checkout. Python 3.10+, Git, and GCC with C++17/libstdc++ are the baseline.
The CLI uses only Python's standard library and bounded compiler/test
subprocesses. Missing tools are errors, never implicit installations.

## Acquire, inspect, search

```sh
python3 tools/toolkit.py doctor --json
python3 tools/toolkit.py sync --all --dry-run
python3 tools/toolkit.py sync --all
python3 tools/toolkit.py sync --source acl kactl --offline
python3 tools/toolkit.py validate --sources --catalog
python3 tools/toolkit.py search "centroid decomposition"
python3 tools/toolkit.py search "" --category "Data structures" --priority advanced
python3 tools/toolkit.py search "" --status stale --json
python3 tools/toolkit.py index --output-dir build
```

`sync --all` selects nine enabled source records; tourist/jiangly are disabled.
Never change the lock just to avoid a failed fetch. Existing dirty,
wrong-origin, wrong-pin, or conflicting paths are preserved and rejected.
Do not manually edit the clones. Submodules are recorded but not initialized.
Suisen's upstream ACL gitlink is not the toolkit's independently pinned ACL.

Search is case-insensitive across names, aliases, tags, taxonomy and authored
contract/reason/notices text. Filters are conjunctive. Empty matches succeed
with an empty result. Priorities describe curriculum, not validation: a
`core` study reference is not necessarily in an integration profile.
Original40 and compact12 are unchanged; expansion36 and expanded76 are
separate explicit selections, not additional meanings of `--all-core`.

The index contains relative URL-escaped links to acquired files. Missing whole
optional checkouts appear as unavailable without fake local links; a broken
declared path in an existing checkout fails validation. Article links open raw
Markdown. This does not rebuild CP-Algorithms' MkDocs website or its plugins,
remote images, math rendering, or internal article navigation.
Original educational references instead link to specific sections beneath
`docs/techniques/`, with local authorship and a content hash. They are never
presented as upstream acquisitions or tested algorithms.

## Test and select

```sh
python3 tools/toolkit.py test --all-core --seed 1
python3 tools/toolkit.py test --selection-profile notebook/profiles/expansion.json --seed 1
python3 tools/toolkit.py test --selection-profile notebook/profiles/expanded.json --seed 1
python3 tools/toolkit.py test --entry dsu fenwick --profile baseline --seed 42
python3 tools/toolkit.py test --entry dsu --profile sanitizers
python3 tools/toolkit.py notebook --profile notebook/profiles/compact.json
python3 tools/toolkit.py notebook --profile notebook/profiles/core.json --output-dir build/core
python3 tools/toolkit.py notebook --profile notebook/profiles/expansion.json --output-dir build/expansion
python3 tools/toolkit.py notebook --profile notebook/profiles/expanded.json --output-dir build/expanded
python3 tools/toolkit.py validate --links
```

Each example is a complete independent usage program, not a common judge I/O
format. To compile one directly, supply local helpers and its exact upstream
root(s), for example:

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude \
  -Iupstream/acl/864245a00b00dd008d1abfdc239618fdb7d139da \
  examples/dsu.cpp -o build/dsu
./build/dsu
```

Use the runner for dependency discovery, consistent flags, tests, and
evidence. It does not add `-march=native`, AVX2, fast-math, or hidden optimizations.
See [testing](testing.md) for evidence and optional sanitizer requirements.

Create an ordered custom profile inside the repository:

```json
{
  "schema_version": 1,
  "name": "Graph practice",
  "description": "Two independent graph examples",
  "entries": ["dijkstra", "dsu"],
  "include_study_references": false,
  "print_options": {}
}
```

Pass its path to `notebook --profile`. Entries must exist and be unique, and
the selection must be nonempty. Test `--selection-profile` accepts this same
ordered profile, but every selected entry must be implemented. It is mutually
exclusive with `--entry` and `--all-core`. Test `--profile` independently
selects compiler flags (`baseline` or `sanitizers`).
Default profiles require current passing baseline evidence and available
permitted content. Explicit `include_study_references: true` additionally
allows reference-only entries with prominent study labels; it does not allow
missing code, unknown permissions, failed implementations, or silent omission.
Original prose is permitted only through this explicit study opt-in and its
own copy policy. It remains labeled educational, with no passing-code badge
or invented upstream license. Copied Markdown is literal fenced source;
relative links inside it are not treated as generated-output links.

Notebook sections preserve profile order, contracts, complete upstream/original
snippet content, and separate standalone examples. A deduplicated dependency
appendix follows literal local includes, including extensionless ACL wrappers.
System standard headers are listed rather than copied. Conditional include
branches are conservatively inspected; nonliteral or unresolved local includes
fail. The notebook is not an include-flattening compiler or one-file merge.
Preserve the shown directory layout or consciously adapt the includes.

HTML works through `file://` with no fetch, CDN, font download, or server.
Use browser Print for PDF output; pagination is browser-dependent and no
automatic page-limit truncation is applied. Identical metadata, availability,
source content, and evidence produce byte-identical output.
The 12-entry compact profile is a smaller selection, not a guaranteed short
contest sheet. Complete contracts/dependencies/notices can still occupy many
pages; see the [observed print sizes](testing.md#initial-delivery-checkpoint)
before choosing a profile for a page-limited contest.

## Updating deliberately

1. Review the proposed full revision, authorship, licenses/file exceptions,
   dependencies, and portability before editing `sources/lock.json`.
2. Sync the new content-addressed path. Old checkouts remain untouched; the
   toolkit never removes them automatically.
3. Recheck every affected catalog reference, contract, helper and dependency.
4. Run affected local tests, inspect failures/seeds, and rebuild index/notebook.
5. Only after review, use `test --entry ... --record`, `--all-core --record`,
   or `--selection-profile notebook/profiles/expanded.json --record` to
   replace reviewed evidence. This is an explicit tracked-file change.

`build/validation.json` overrides reviewed results, including failed reruns.
Stale fingerprints cannot retain `tested` badges. Deleting a failure report
to hide it is not a substitute for fixing/retesting an integration.

For a fresh-snapshot installation exercise, use a new directory inside this
worktree's `build/` area, export the intended working-tree inputs, and use
`--root`. Include the pending implementation files: before they are committed,
`git archive HEAD` does not contain this toolkit. Report this honestly as a
fresh exported-working-tree snapshot, not reproduction from a committed
release. Acquire the nine exact sources there, then repeat all normal commands
offline. Do not depend on another checkout's source symlinks or build products.
Use the expanded76 test command and generate all four profiles when reproducing
the expansion, rather than assuming that `--all-core` now covers new entries.

## Errors

Exit 0 means the complete request succeeded. Exit 2 identifies malformed
input/schema; exit 1 identifies execution/acquisition/test failures.
Diagnostics name the entry/source/path on stderr; `--json` supplies structured
stdout. Read failures and broken declared links are surfaced, not replaced by
empty sections. Do not request output inside protected source directories.
