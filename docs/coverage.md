# Coverage without blanket certification

The catalog is a finite, individually enumerated preparation inventory, not a
claim to contain every algorithm or solve every problem. Its authoritative
records are in [`catalog/algorithms.json`](../catalog/algorithms.json).
Each semicolon-separated target in the approved coverage plan has its own
entry; distinct compound operations such as queue/stack/deque and permutation
ranking/unranking are split. Category labels are not substitutes for entries.

## Structural counts

The delivered inventory contains **265 individual entries**:

- **76 implemented integrations**, selected exactly by `expanded.json`;
- **184 reference-only entries**: 161 pinned references and 23 original
  educational references;
- **Five missing study targets**, each with an explicit reason.

These are implementation-state counts, **not passing-test counts**. Readiness
is computed from current local evidence; use the commands below for tested,
failed and stale counts on your checkout. A source file containing an upstream
“tested” comment still does not acquire a local tested badge.

| Topic | Implemented | Reference-only | Missing |
|---|---:|---:|---:|
| Fundamentals, search, sorting | 4 | 12 | 0 |
| Data structures | 12 | 19 | 1 |
| Graph connectivity and traversal | 6 | 8 | 1 |
| Shortest paths and spanning trees | 6 | 8 | 0 |
| Trees | 4 | 8 | 0 |
| Flow, cuts, and matching | 5 | 9 | 0 |
| Strings | 8 | 7 | 0 |
| Dynamic programming | 8 | 13 | 0 |
| Number theory | 8 | 16 | 0 |
| Combinatorics | 1 | 15 | 0 |
| Linear algebra | 3 | 8 | 0 |
| Transforms, polynomials, FPS | 2 | 17 | 0 |
| Computational geometry | 5 | 14 | 0 |
| Game theory | 2 | 5 | 0 |
| Numerical methods | 1 | 6 | 0 |
| Randomized algorithms | 0 | 6 | 1 |
| Problem-solving references | 1 | 13 | 2 |
| **Total** | **76** | **184** | **5** |

A zero in the missing column only describes this enumerated inventory. It is
not a topic-completeness or correctness claim. Source-reference counts count an
entry at most once per source; entries may cite multiple sources, so this
table is deliberately not additive:

| Source | Entries referencing that source |
|---|---:|
| ACL | 19 |
| KACTL | 30 |
| CP-Algorithms articles | 77 |
| CP-Algorithms auxiliary | 6 |
| Nyaan | 51 |
| Luzhiled | 26 |
| Suisen | 4 |
| Benq | 4 |
| ecnerwala | 3 |

Integrations contain independently authored helpers and adapters (some also
depend on upstream headers). Their local code is not attributed to a fictitious
upstream repository. Source references and include-source dependencies have
separate roles. Original educational notes separately record repository-local
authorship and content hashes; they have no invented upstream SHA or license.

## Status meanings

| Display | Meaning |
|---|---|
| `missing` | No concrete verified reference/integration for this target; read its explicit reason. |
| `reference-only` | A specific pinned code/article reference or original educational section exists, but no local ready-to-use integration/evidence is claimed. |
| `implemented` | Actual local example/helper/test paths and full contract exist; current matching passing evidence is absent. |
| `tested` | Local baseline evidence matches relevant current sources, dependencies, helpers, examples, tests, metadata and runner inputs. |
| `failed` | A current local run failed; an older reviewed pass does not override it. |
| `stale` | Evidence no longer matches current content/profile/dependencies or cannot be reproduced from available inputs. |

`core`/`advanced` is curriculum priority, independent of readiness.
`test --all-core` uses the exact 40 IDs from the core profile, not every
study entry with core priority. Source availability and copy permission are
also independent: unavailable code is not usable, and public-but-unlicensed
content cannot be copied into a notebook.
`test --selection-profile notebook/profiles/expanded.json` selects all76;
`expansion.json` selects only the new36. The compiler `--profile` is separate.

```sh
python3 tools/toolkit.py index --json
python3 tools/toolkit.py search "" --status tested --json
python3 tools/toolkit.py search "" --status failed --json
python3 tools/toolkit.py search "" --status stale --json
python3 tools/toolkit.py search "" --status missing --json
```

The index JSON reports live individual counts by topic, displayed state and
source. Generating it does not run algorithms or promote implementation state.
Original/tested integrations coexist with educational references and gaps.

## Selection boundary

The original 40-entry core is preserved in
[`notebook/profiles/core.json`](../notebook/profiles/core.json), corresponding
to C01–C40 in the [approved coverage plan](../specs/001-comprehensive-icpc-toolkit/coverage.md).
The compact profile selects exactly:

`dsu`, `fenwick`, `segtree`, `dijkstra`, `lca`, `max-flow`,
`bipartite-matching`, `prefix-function`, `crt`, `modular-convolution`,
`convex-hull`, `gaussian-elimination`.

The exact36 additions and their supported domains are listed in the
[expansion scope](../specs/002-core-coverage-expansion/coverage.md).
`expansion.json` contains those36; `expanded.json` contains original40 then36.
Lowlink counts as one integration, not separate bridge/articulation/block
wrappers. Digit DP specifically counts no-equal-neighbor decimal numbers with
a digit-sum residue; interval DP solves optimal adjacent merges. Neither is a
generic solver for all digit/interval recurrences. Static Mo implements
range-distinct queries, not temporal Mo. Pollard rho can explicitly exhaust its
retry budget; rolling-hash equality is a probabilistic filter.

## Every original gap, with residual omissions

The [per-gap audit](gap-audit.md) and its
[machine-readable history](../catalog/gap-audit.json) retain all43 original
IDs, reasons, final states, provenance and rationale: **seven integrations,
eight pinned references, 23 original educational references, five residual
gaps**. Original prose includes examples, invariants and pitfalls, not tested
solver badges. Johnson reweighting remains an explanatory technique.

The remaining five targets are **Mo with updates, minimum cycle basis,
randomized polynomial identity testing, multidimensional dominance, and CDQ
divide-and-conquer**. Related static Mo, arbitrary cycle spaces, string hashes,
range containers, and time-tree rollback do not establish these targets.
The [contest workflow](contest-workflow.md) remains complementary preparation
advice rather than a source of implicit implementation coverage.

Eight references remain copy-blocked: Benq's Euler-tour dynamic tree, general
weighted matching and matroid intersection have unresolved inherited terms;
its minimum-cost circulation file lacks a file-level license marker accepted
by the current selective-copy policy;
KACTL multinomial/3D hull/polygon union have unclear per-file permissions;
KACTL numerical root subdivision depends on unlicensed `Polynomial.h`.
The numerical root reference is not a certified exact root-isolation solver.
KACTL `WeightedMatching.h` is bipartite assignment, so it is not credited as
general weighted matching.

Raw CP-Algorithms article Markdown stays educational and reference-only.
Optional ecnerwala/auxiliary C++23 code is not promoted to GNU C++17 core
readiness. Study notebook opt-in still checks all copied dependency
permissions and rejects unresolved includes instead of silently omitting them.

Tourist/jiangly remain disabled unresolved sources and `ksan` is excluded.
See [sources](sources.md), [portability](portability.md), and
[testing](testing.md) before interpreting a source's upstream reputation or
test comments as evidence about this toolkit.
