# Competitive-programming field guide

A pinned offline source shelf, an explicitly bounded **76-integration GNU
C++17 toolkit**, and an individual algorithm/reference/gap catalog. Acquisition,
integration, and current local test evidence are different states; no upstream
repository is certified wholesale.

The expanded inventory has **265 entries: 76 implemented integrations,
184 reference-only entries, and five explicit gaps**, backed by nine unchanged
pinned source repositories. The original40 baseline and compact12 selections
remain unchanged; expansion36 and expanded76 are explicit additional profiles.
See [coverage](docs/coverage.md) and the [audit of all43 old gaps](docs/gap-audit.md).

The user selected a C++17-compatible core for ETCPC. **ETCPC's exact current
compiler has not been publicly verified.** This baseline is GCC/libstdc++ with
GNU facilities such as `bits/stdc++.h`, not compiler-independent ISO C++.
CP-Algorithms auxiliary and ecnerwala's C++23 material remain a separate
reference shelf.

## Start here

Prerequisites: Linux, Git, Python 3.10+, and `g++` supporting C++17. No pip,
npm, CMake, TeX, or upstream test framework is needed.

```sh
python3 tools/toolkit.py doctor
python3 tools/toolkit.py sync --all
python3 tools/toolkit.py sync --all --offline
python3 tools/toolkit.py validate --sources --catalog
python3 tools/toolkit.py test --selection-profile notebook/profiles/expanded.json --seed 1
python3 tools/toolkit.py index
python3 tools/toolkit.py notebook --profile notebook/profiles/compact.json
python3 tools/toolkit.py validate --links
```

Only online `sync` accesses the network. It acquires nine exact pins beneath
`upstream/<source-id>/<revision>/`, never resets a modified checkout, and does
not initialize upstream submodules or execute upstream setup scripts.

Open `build/index.html` directly in a browser (`file://` works). Search is
embedded and offline; `build/index.md` is the plain-text alternative. The
notebook is `build/notebook/notebook.html` and `.md`. Browser Print can produce
a PDF; inspect wrapping and your contest's actual reference/page rules.
The compact profile selects 12 entries, not a fixed page count; complete
dependencies and notices are retained.

```sh
python3 tools/toolkit.py search "2-SAT"
python3 tools/toolkit.py search "persistent segment tree" --status tested
python3 tools/toolkit.py search "" --status missing --json
python3 tools/toolkit.py test --entry dsu fenwick --seed 42
python3 tools/toolkit.py test --all-core --seed 1
python3 tools/toolkit.py test --selection-profile notebook/profiles/expansion.json --seed 1
python3 tools/toolkit.py notebook --profile notebook/profiles/core.json --output-dir build/core
python3 tools/toolkit.py notebook --profile notebook/profiles/expansion.json --output-dir build/expansion
python3 tools/toolkit.py notebook --profile notebook/profiles/expanded.json --output-dir build/expanded
```

`--all-core` deliberately retains the original40 selection. Test
`--selection-profile` chooses algorithms; test `--profile` chooses compiler
flags (`baseline` or `sanitizers`). The expanded notebook is not the default
compact notebook and is not intended to fit a universal contest page limit.

Notebook generation refuses unavailable, untested, failed, stale, or
permission-blocked selections. Changing relevant code, contracts, pins, tests,
or the runner invalidates old evidence. `test --record` explicitly updates
reviewed `catalog/validation.json`; ordinary tests write only
`build/validation.json`.

## What to read

- [Usage and deliberate updates](docs/usage.md)
- [Sources, authors, pins, and permissions](docs/sources.md)
- [Coverage and honest status meanings](docs/coverage.md)
- [Exact36 expansion scope](specs/002-core-coverage-expansion/coverage.md)
- [Original-to-final gap audit](docs/gap-audit.md)
- [Portability and numeric/stack constraints](docs/portability.md)
- [Local tests and evidence](docs/testing.md)
- [Contest workflow](docs/contest-workflow.md)
- [Compact snippet conventions](docs/snippet-style.md)

Tourist and jiangly remain disabled, unresolved source records; `ksan` is
excluded as requested. The 23 original educational references under
`docs/techniques/` remain reference-only, not generic tested solvers.
Educational Markdown is linked as educational material,
not presented as ready-to-use code or a reconstructed upstream website.
Public availability alone does not permit copying.

## Repository layout

`sources/lock.json` locks provenance; `catalog/algorithms.json` records the
single algorithm catalog; `catalog/gap-audit.json` preserves all43 original
gap decisions. `examples/` contains independently compiled usage
programs; `include/toolkit/` holds small original helpers and a KACTL prelude.
`tests/cpp/` contains local oracle/edge checks; `tests/python/` exercises the
offline tools and failure cases. Profiles under `notebook/profiles/` select
ordered entries. Acquired sources and generated `build/` output are ignored.

Run tooling checks with `python3 -m unittest discover -s tests/python`.
No repository-wide license has been selected for original project code;
upstream permissions and per-file notices remain distinct.