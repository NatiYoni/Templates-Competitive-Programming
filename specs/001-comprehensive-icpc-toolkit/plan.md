# Implementation Plan: Comprehensive ICPC Preparation Toolkit

**Branch**: `natiyoni-comprehensive-icpc-toolkit` | **Date**: 2026-09-12 |
**Spec**: [spec.md](spec.md)

**Input**: `specs/001-comprehensive-icpc-toolkit/spec.md`

**Approval state**: Approved on 2026-09-12. The user explicitly selected the
C++17-compatible core for ETCPC. ETCPC's exact compiler remains unverified;
newer-standard material stays separate. Compact snippet presentation does not
replace documented contracts, permissions, dependencies, or local testing.

**Delivery state**: Implemented in this worktree without committing or publishing.
All T001-T036 tasks are complete; the [acceptance results](checklists/requirements.md)
record the actual source, integration, reference, and gap counts. Fresh-install
reproduction used an exported working-tree snapshot, not committed `HEAD`.

## Summary

Deliver three separate layers: an unmodified pinned source shelf; 40 locally
tested C++17 integrations; and a broad, honest offline study/contest index.
Acquire nine verified repositories, retain unresolved tourist/jiangly source
records, and exclude `ksan`. Enumerate all other coverage as references or gaps.
Use official GitHub Spec Kit 1.0.1, already scaffolded in this worktree.

## Technical Context

**Language/Version**: C++17 core; Python 3.10+ standard library; small local
HTML/CSS/JavaScript assets.

**Primary Dependencies**: Git, GCC with C++17, locked source repositories.
Optional installed Clang/sanitizers. No required pip/npm/CMake/TeX stack.

**Storage**: Committed JSON lock/catalog/profile/evidence; ignored
`upstream/<id>/<full-sha>/` checkouts and `build/`.

**Testing**: Python `unittest`, isolated C++ examples/oracle binaries,
metadata/link checks, and a clean-checkout acceptance exercise.

**Target Platform**: Linux/GCC baseline; extra platforms require extra evidence.

**Project Type**: Local CLI, curated C++ examples, and generated documentation.

**Performance Goals**: Search indexed metadata rather than scanning/rebuilding
entire archives; bounded test processes. No unmeasured runtime or size promise.

**Constraints**: Pinned non-destructive acquisition, explicit permissions,
offline normal operation, no implicit upstream execution or CPU tuning.

**Scale/Scope**: Nine acquisitions, two disabled source records, 40 tested
core entries, the entire named coverage inventory, configurable notebook.

## Constitution Check

| Gate | Design decision | Result |
|------|-----------------|--------|
| Evidence before confidence | Acquisition/integration/evidence remain separate; hashes invalidate badges | Pass by design |
| Provenance/permissions | Source lock, file-level exceptions, blocked unclear copying | Pass by design |
| Non-destructive acquisition | Content-addressed clones, atomic staging, no resets | Pass by design |
| Bounded reliable curation | 40 named integrations; reference/missing beyond them | Pass by design |
| Portability/contest constraints | C++17 default, explicit extensions, editable selection | Pass by design |
| Offline/minimal dependencies | Standard library generator, local assets/relative links | Pass by design |

Post-design re-evaluation: no exceptions required. A core licensing/portability/
correctness failure blocks completion; it cannot be relabeled to meet a count.

## Project Structure

### Documentation (this feature)

```text
.specify/memory/constitution.md
.specify/NOTICE.md
.specify/templates/ and scripts/       # official infrastructure
.github/skills/speckit-*/              # official integration
specs/001-comprehensive-icpc-toolkit/
  spec.md
  plan.md
  research.md
  coverage.md
  data-model.md
  contracts/cli.md
  quickstart.md
  tasks.md
  checklists/requirements.md
```

### Source Code (implemented)

```text
README.md
.gitignore
sources/lock.json
catalog/{algorithms,validation}.json
notebook/profiles/{compact,core}.json
include/toolkit/                      # small originals/compatibility prelude
examples/<entry-id>.cpp
tests/cpp/<entry-id>_test.cpp
tests/python/test_*.py
tools/toolkit.py
tools/lib/{models,acquire,catalog,validation,render}.py
tools/assets/{style.css,search.js}
docs/{sources,usage,portability,testing,coverage,contest-workflow}.md
upstream/<source-id>/<sha>/           # ignored, unmodified
build/                               # ignored
```

**Structure Decision**: A small CLI and independent examples, not an application
framework or copied mega-library. Keep upstream APIs visible; adapters explain
preconditions rather than pretending all libraries share one interface.

## Phase 0: Research Decisions

Evidence and candidate pins are in [research.md](research.md).
CP-Algorithms auxiliary is genuine and distinct from the article repository.
Current auxiliary and ecnerwala code use C++23. Benq has both notebook and
solution-archive areas. KACTL has mixed per-file permissions; selected core
headers have explicit licenses. Its unlicensed Manacher/TopoSort are not copied.
Nyaan has AVX2-only files; Suisen's CC0 declaration is in its README and some
headers need ACL. Tourist/jiangly remain unverified rather than silently guessed.

## Phase 1: Design and Contracts

### A. Source acquisition

Create `sources/lock.json` with the nine full research pins, provenance, license
evidence/exceptions, compiler notes, submodules/dependencies, and explicit
decisions. Recheck those revisions during implementation; do not advance tips.

Derive destination paths only from validated IDs/SHAs. Acquire absent sources
in unique staging directories under `upstream/`, fetch the exact commit,
check out detached, verify origin/HEAD/content, and atomically publish the
checkout. Use bounded subprocess argument arrays without shell evaluation.
No implicit upstream hooks/builds/tests or recursive submodule operations.
Record unacquired gitlinks; separately pinned ACL covers relevant dependencies.

Existing destinations must match origin and pin and have no modifications,
untracked additions, or ignored generated additions. Otherwise fail without
resetting/deleting them. Reject traversal, escaping symlinks, unexpected
repositories, and writer conflicts. Cleanup only the operation's own resolved
staging directory. Missing/unfetchable pins fail; no silent mirror/tip/full-
history fallback. Offline mode only inspects existing checkouts.

Pin changes are deliberate reviewed lock edits. New pins use new directories;
old checkouts are neither altered nor automatically removed.

### B. Catalog and evidence

Split every named inventory item in [coverage.md](coverage.md) into its own
stable entry. Verify actual source paths before crediting references; otherwise
record `missing` with reason. Category/priority is independent of readiness.

Source-only headers/articles are `reference-only`. A real local example/adapter
is `implemented`. A displayed `tested` badge requires matching local evidence.
Represent acquisition and license blocking separately from algorithm status.

Record dependencies, source SHA/files, contract, compiler profile, example,
and test IDs. Relative links are computed from output location and URL-escaped.
Fail on broken declared paths; unavailable optional sources get an honest
disabled/external record, not a fabricated local link.

### C. Core integrations

Implement the 40 entries in independent translation units. Use direct ACL
headers with the locked root on the include path. Selected KACTL snippets use
a small original prelude with only needed standard includes, aliases/macros,
and unchanged upstream relative includes. Do not copy its unlicensed contest
template. Do not promise that global names/macros from all libraries coexist.

Each entry gets a contract, one example, source/dependency hashes, and local
tests. Reflect actual limits: uppercase nonempty Aho-Corasick patterns, fixed
sieve capacity, recursive root-zero LCA, bounded geometric products, and
floating-point tolerances. Narrow contracts or explicitly attribute an adapter
rather than editing the upstream clone. Validate unsupported input at the
example/CLI boundary; raw contest headers retain documented preconditions.

### D. Validation harness

Compile selected examples/tests with explicit include paths,
`-std=c++17 -O2 -Wall -Wextra`, and per-test timeouts. Optional sanitizer/Clang
profiles are separate evidence. No native/AVX2/fast-math defaults.

Use at least 200 reproducible small oracle cases per entry where randomized/
exhaustive comparison applies, plus the specified boundary cases. Numeric
methods use analytic or constructed-solution checks with explicit tolerances.
Record case counts/seeds, compiler/version/flags/platform, and content hashes
for source closure, helpers, examples, tests, and relevant entry metadata.

Current results go to `build/validation.json`; an explicit recording command
updates reviewed `catalog/validation.json`. Failure/staleness overrides a
historical pass. Do not mix nondeterministic timestamps/run logs into generated
document inputs. Compiler-only checks are not algorithm-test evidence.

### E. Offline index and notebook

Use standard-library generation/escaping and local assets. Embed search data
so opening `build/index.html` through `file://` needs no fetch/server/CDN/font.
Generate Markdown as a readable alternative. Search covers names, aliases,
tags, and authored notes.

Link raw upstream code/article files with explicit format labels; licensed
previews are escaped text, never executable upstream HTML. Full reconstruction
of CP-Algorithms' MkDocs plugins/math website is not promised: its article
Markdown remains locally accessible.

`core.json` selects all 40 entries. `compact.json` selects 12:
`dsu`, `fenwick`, `segtree`, `dijkstra`, `lca`, `max-flow`,
`bipartite-matching`, `prefix-function`, `crt`, `modular-convolution`,
`convex-hull`, `gaussian-elimination`.
Custom profiles control order and opt into clearly labeled study references.

Notebook output includes usage, assumptions, permitted complete snippet
content, and deduplicated dependencies/notices. It must not silently omit
include prerequisites or license comments. Browser print styles provide a PDF
path without assuming page limits or identical browser pagination.

## Phase 2: Executable Delivery Slices

See [tasks.md](tasks.md) for exact paths, dependencies, and acceptance mappings.

1. Establish shared lock/catalog contracts and minimal test scaffolding.
2. Deliver US1: safe pinned acquisition and provenance documentation.
3. Deliver US2: individual coverage entries and offline searchable index.
4. Deliver US3: all 40 integrations with oracle evidence and contracts.
5. Deliver US4: configurable notebook and practical preparation notes.
6. Deliver US5: README/update workflow and clean-checkout/offline acceptance.

Rendering can use fixtures in parallel with acquisition. After contracts/pins,
different algorithm families can be implemented independently. One owner merges
catalog/manifest changes to avoid concurrent edits to shared JSON.

## Acceptance and Stop Conditions

The quickstart and requirement/task mapping define completion. Never promote a
missing/failed entry to reach a count. A core blocker requires an explicit
reported adjustment and approval; optional references may remain unavailable.
The initial request does not authorize toolkit execution, a commit, push, or PR.

## Complexity Tracking

No constitution violations. The original repository had only a README and no
test/build conventions; the small local harness is necessary. Content-addressed
clones and content-bound evidence support non-destructive updates and honest
readiness claims without a database or application framework.
