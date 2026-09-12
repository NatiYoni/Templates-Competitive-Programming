# Local validation and evidence

The core targets **GNU C++17 with GCC/libstdc++**, including compact
`bits/stdc++.h` snippets. The exact ETCPC compiler is not verified. Python 3.10+
and Git are required for the full toolkit; there are no pip/npm/TeX dependencies.
`doctor` reports installed baseline tools without writing or downloading anything:

```sh
python3 tools/toolkit.py doctor
python3 -m unittest discover -s tests/python -v
```

Python tests create isolated fixtures beneath `build/python-fixtures/`, remove
their own fixtures afterward, and use local Git repositories for acquisition
tests. They do not fetch the production sources. Compiler-dependent fixtures
explicitly skip when GCC is unavailable; a skipped Python fixture is **not**
passing algorithm evidence.

## Validate metadata and acquired files

```sh
python3 tools/toolkit.py validate
python3 tools/toolkit.py validate --sources --catalog --links
```

Default validation checks lock/catalog/evidence structure. `--catalog` also
checks declared local paths, acquired reference paths, and compiler-resolved
integration dependencies. `--sources` explicitly verifies all enabled source
pins **offline**, including origins, HEAD, and modified/untracked/ignored files.
It never fetches missing sources. `--links` checks generated local links and
anchors. Run `index` and `notebook` before validating their generated links.

Only an explicitly requested `sync` without `--offline` may use the network.
No command executes upstream setup scripts, upstream test frameworks, or
recursive submodule acquisition.

## Run independent examples and oracle tests

```sh
python3 tools/toolkit.py test --entry dsu fenwick --seed 17
python3 tools/toolkit.py test --all-core --seed 1
python3 tools/toolkit.py test --selection-profile notebook/profiles/expansion.json --seed 1
python3 tools/toolkit.py test --selection-profile notebook/profiles/expanded.json --seed 1
python3 tools/toolkit.py test --all-core --profile sanitizers --seed 1
```

Each example and test is an independent translation unit. The baseline command
uses `g++ -std=c++17 -O2 -Wall -Wextra`, `-I include`, `-I tests/cpp`, the selected
locked `upstream/<source-id>/<full-sha>` include roots, and
`-DTOOLKIT_SEED=<seed>`. No `-DNDEBUG`, native tuning, AVX2, or fast-math is added.
The seed is an integer from 0 through 4294967295.

`--all-core` selects the explicit ordered entries in
`notebook/profiles/core.json` (the 40 integrated core algorithms). It does not
select every catalog topic with `priority: core`: priority is an independent
study/contest classification and can also label unimplemented references.
`--selection-profile notebook/profiles/expansion.json` selects only the new36;
`expanded.json` selects original40 followed by36. This algorithm selector
does not change compiler `--profile`, and rejects references, empty profiles,
duplicates, unknown IDs, and conflicting selectors.

The runner compiles **and runs** each example and test, with a 90-second bound
per compilation/dependency extraction and a 15-second bound per execution.
Tests must assert their comparisons and print exactly one
`OK cases=N seed=N` line with a positive case count and the requested seed.
The core oracle suites use at least 200 small randomized/exhaustive cases where
applicable, together with explicit boundary/analytic cases. The runner verifies
the reported seed/count; the tests themselves define the actual independent
oracles and supported input domains. Compile-only success is not an oracle pass.

The distinct `sanitizers` profile uses `-O1 -g -fsanitize=address,undefined`,
frame pointers, and fail-fast sanitizer settings. Missing capability or a
sanitizer failure is a failure of that requested profile, never a silent fallback.
Sanitizer evidence does not replace the baseline evidence used by notebooks.
The harness executes authored local C++ code, not a security sandbox.

## Current versus reviewed evidence

Ordinary test runs update ignored `build/validation.json`, including failures.
To deliberately update the reviewed report:

```sh
python3 tools/toolkit.py test --all-core --seed 1 --record
python3 tools/toolkit.py test --selection-profile notebook/profiles/expanded.json --seed 1 --record
```

`--record` merges each selected entry/profile into `catalog/validation.json`,
including any failures, while preserving unrelated records. Review the JSON
diff alongside algorithm, contract, source-pin, permission, and dependency
changes before committing it. Recording never changes a lock pin.

Evidence binds the complete entry metadata, source pins/permission metadata,
helper/example/test contents, and the full transitive repository-local include
closure extracted by compiler `-M`. Local headers remain fingerprinted even
when a pragma classifies them as system headers. Both lexical and resolved
local paths are retained, symlink targets are recorded, and canonical upstream
dependencies must match enabled lock pins. Catalog dependency validation uses
the same checks as the runner.

Include/toolchain override environment variables (including `CPATH` and
`CPLUS_INCLUDE_PATH`) are removed consistently during compiler discovery,
dependency extraction, and compilation. Only external headers beneath the
compiler's discovered default system include roots are accepted; other external
project dependencies fail explicitly, including system-marked headers. These
system roots, runner code/profile, seed, compiler version, flags, and platform
are recorded as evidence inputs. Compiler/system headers are represented by
the toolchain identity rather than copied into the toolkit. Rechecking the
closure also catches newly added headers that shadow existing includes.

A current build result overrides reviewed history for the same entry/profile:
a failure cannot revive an older pass. Changed relevant inputs display `stale`
until rerun; a missing compiler/dependency also prevents reuse of a tested badge.
Records deliberately contain no wall-clock timestamps. Readiness is
content-bound local evidence, not a signature or upstream certification.

## Installation acceptance

The Python acceptance fixture exercises CLI, evidence, index, notebook, and
staleness without acquiring the production source shelf. A full installation
check must additionally acquire the nine locked sources, run all76 integrations,
generate compact12/core40/expansion36/expanded76 notebooks, and verify local
links. Keep the original40 command available as a reproducible baseline.

While implementation files remain uncommitted, an export of the working tree
is a **fresh snapshot installation**, not reproduction from committed `HEAD`.
Record which snapshot was exercised and retain its evidence; do not imply
pending files were present in the branch's committed history. Source disk
usage and compiler/platform versions are observations from that run, not
portable size or toolchain guarantees.

### Initial delivery checkpoint

At the initial feature001 delivery, the recorded baseline contained **40 passing
entries and 27,634 oracle cases**, seed **1729**, with at least 262 cases per
entry. That historical snapshot was produced with GCC 15.2.0 on Linux x86_64 using the documented
C++17 flags. The existing Python infrastructure suite passed all **73 tests**.
These are results for that content-bound snapshot, not future or upstream-wide
certification; live readiness still comes from the evidence rules above.

A separate working-tree snapshot independently acquired all nine exact
repositories from their origins, repeated source verification offline, and
ran all 40 integrations. Its evidence fingerprints matched this installation.
Both installations generated byte-identical index/compact/core HTML, Markdown,
CSS, and JavaScript artifacts. Source, catalog, and generated-link validation
passed. Real `file://` browser runs found 265 entries, 40 tested entries,
43 missing entries, and one 2-SAT match with no remote page resources or script
exceptions.

Both notebook profiles also printed successfully to A4 PDF. All 40 compact
and 114 core code/prose blocks survived text extraction without dropped
non-whitespace content. The observed output was **57 pages for compact and
169 pages for core**, including complete dependencies, contracts, and notices.
Compact means a 12-entry selection, not a page-bounded contest sheet. Adjust
selection and inspect your browser's print preview against the contest rules.
PDFs are optional browser exports, not byte-deterministic generator outputs.
Chrome, Node, and existing PDF inspection utilities were used for this extra
acceptance exercise; they are not toolkit runtime prerequisites.

### Expansion delivery checkpoint

The feature002 recorded baseline contains **76 passing entries and 159,783
oracle cases**, seed **1729**, using the same GNU C++17/GCC 15.2.0 environment.
The new36 contribute **132,149 cases**, with at least 256 per addition. The
Python infrastructure suite passes **101 tests**. Six focused higher-risk
suites additionally ran with AddressSanitizer/UndefinedBehaviorSanitizer;
this is not a whole-toolkit sanitizer claim or replacement for baseline
evidence.

Feature002 adds36 distinct integrations, not extra counts for thin wrappers.
Its oracle suites include exhaustive small modular logs/primality, exhaustive
geometry degeneracies, naive signed paths and tree aggregates, brute-force
flows/assignments/subsets, cubic merge recurrences, and direct substring/window
comparisons. Failure cases cover arithmetic and node budgets, rejected graphs,
hash-context mismatches and exhausted Pollard-rho retries. These tests establish
only the declared domains, not universal correctness or stack sufficiency.

Current all76 results belong to `catalog/validation.json`; the feature001
numbers above remain historical. The [gap audit](gap-audit.md) records all43
former gaps independently of algorithm evidence. Its23 original educational
references have no oracle suite or tested-solver badge.

A fresh working-tree export independently acquired the same nine pins and
ran the complete expanded76 command offline. All76 evidence fingerprints and
case counts matched this installation. Nineteen generated HTML/Markdown/CSS/JS
files, including an explicit original-reference study notebook in a nested
output directory, were byte-identical across installations. Original
compact/core HTML, Markdown and CSS also remain byte-identical to v1.

Actual `file://` browser runs in both installations found265 entries,
76 tested,184 reference-only and five missing, with functioning search,
original-reference discovery, exact notebook order, no script exceptions and
no remote page resources. All printed code/prose blocks retained their
non-whitespace content. Observed A4 exports are:

| Selection | HTML / Markdown directory | Entries | Pages | Code/prose blocks |
|---|---|---:|---:|---:|
| Original compact | `build/notebook/` | 12 | 57 | 40 |
| Original core | `build/core/` | 40 | 169 | 114 |
| Expansion only | `build/expansion/` | 36 | 124 | 113 |
| Expanded toolkit | `build/expanded/` | 76 | 288 | 220 |

Each directory contains `notebook.html`, `notebook.md`, and an optional
browser-exported `notebook.pdf`. These page counts are observations, not limits
or cross-browser guarantees. The expanded profile did not enlarge compact12.
The index is `build/index.html` / `build/index.md`; study acceptance output is
`build/study/originals/notebook.html` / `.md`.

Final acceptance records the exported working-tree input hashes, independent
baseline reports, browser/PDF observations, and generated hashes in session
artifacts. The disposable fresh installation is removed after archival.
No committed-HEAD release, commit, push, or publication is implied.

## Diagnose failures

- `missing`/`reference-only`: no runnable integration; select an implemented
  entry or finish its declared example and tests.
- Missing pinned include root: explicitly run `sync --source ID`, then rerun.
- Modified/wrong-origin checkout: preserve and inspect it; the toolkit will
  not reset, delete, or replace your work.
- Compiler failure: inspect the emitted command and diagnostics; keep the
  declared C++17 baseline instead of silently enabling a newer standard.
- Assertion/oracle mismatch: reproduce with the reported `--entry` and `--seed`.
- Stale evidence: review changed code/contracts/pins and rerun, rather than
  manually editing a fingerprint or reusing a historical badge.

All commands accept `--root PATH` and `--json` before or after the command.
JSON output has `command`, `ok`, and structured `messages`; diagnostics also go
to stderr. Exit 0 means complete success, 2 means invalid input/schema, and 1
means a prerequisite/acquisition/compilation/runtime failure. Search with no
matches succeeds with an empty result.
