---
description: "Executable tasks for the comprehensive ICPC preparation toolkit"
---

# Tasks: Comprehensive ICPC Preparation Toolkit

**Input**: `specs/001-comprehensive-icpc-toolkit/`

**Prerequisites**: Implementation approved on 2026-09-12 with the user's
C++17-compatible ETCPC baseline; spec/plan, research, coverage, data model,
CLI contracts, and quickstart are established.

**Tests**: Explicitly required by the user/specification. Write the relevant
oracle/fixture cases before implementation and observe the appropriate initial
failure. Do not install a framework when standard-library/C++ checks suffice.

**Organization**: Official Spec Kit task format, grouped by user story.
Checked tasks reflect completed work, not merely generated files. Final
content-matched evidence and end-to-end acceptance remain separate gates.
Operational status is also tracked in the session SQL database.

**Completion**: T001-T036 are complete for the approved bounded delivery.
See [acceptance results](checklists/requirements.md) and
[evidence details](../../docs/testing.md#initial-delivery-checkpoint).
Fresh-install acceptance used an exported working-tree snapshot because no
commit or publication was authorized. References and explicit coverage gaps
remain separate from the completed 40-entry implementation requirement.

## Phase 1: Setup and Shared Contracts

- [x] T001 Define ignore rules for `upstream/`, `build/`, and transient Python artifacts in `.gitignore`; establish the proposed directories without changing upstream or unrelated files.
- [x] T002 Write schema/state/path fixture tests in `tests/python/test_models.py` for source, algorithm, evidence, and notebook contracts, including duplicates, bad paths, missing references, and incompatible states.
- [x] T003 Implement standard-library validation and shared types in `tools/lib/models.py`, plus `tools/toolkit.py` command dispatch/common errors, satisfying T002 and `contracts/cli.md`.

**Checkpoint**: Shared contracts fail explicitly and can support independently
testable acquisition, catalog, and runner work.

## Phase 2: User Story 1 - Safe Attributable Acquisition (Priority: P1)

- [x] T004 [US1] Create `sources/lock.json` with the nine exact researched pins, identity/license observations, file exceptions, submodules/dependencies, and disabled tourist/jiangly records; exclude `ksan`.
- [x] T005 [US1] Write `tests/python/test_acquire.py` with isolated local Git fixtures for exact pins, dirty/wrong-origin destinations, missing revisions, failed fetches, traversal/symlinks, staging cleanup, concurrency, offline and dry-run behavior.
- [x] T006 [US1] Implement pinned non-destructive acquisition in `tools/lib/acquire.py` and `sync` dispatch in `tools/toolkit.py`; never reset checkouts, advance lock pins, or run upstream scripts/submodules.
- [x] T007 [US1] Acquire the nine approved sources into `upstream/<id>/<sha>/`, verify exact origin/HEAD/content/notices, and rerun offline verification; surface each partial failure rather than claiming overall success.
- [x] T008 [US1] Write `docs/sources.md` with source classifications, attribution evidence, retained notices, copy restrictions, compiler caveats, source size observations, and excluded/unverified sources.

**Independent Test**: T005 fixture cases plus T007 exact-pin/offline inspection.
Source shelf alone is useful even before the curated examples exist.

## Phase 3: User Story 2 - Concrete Offline Coverage (Priority: P1)

- [x] T009 [US2] Write `tests/python/test_catalog.py` and `tests/python/test_index.py` for all status types, individual counting, aliases/filters, link resolution, paths with spaces, HTML escaping, and unavailable-source display.
- [x] T010 [US2] Populate `catalog/algorithms.json` from every individual name in `coverage.md`, resolving actual pinned paths/permissions; keep unresolved entries missing with reasons and all not-yet-integrated entries reference-only.
- [x] T011 [US2] Implement catalog loading, derived statuses, filtering and CLI search in `tools/lib/catalog.py` using the shared schema and explicit source availability.
- [x] T012 [US2] Implement deterministic HTML/Markdown index output in `tools/lib/render.py` and local `tools/assets/search.js`/`style.css`; embed search metadata for `file://` and validate generated links.

**Independent Test**: A fixture catalog renders/searches all statuses without
network access; real catalog entries trace to source pins or explicit gaps.

## Phase 4: User Story 3 - Forty Tested Core Integrations (Priority: P1)

- [x] T013 [US3] Write `tests/python/test_validation.py` for compile/run failure, timeout, missing compiler/profile, hash invalidation, failed-rerun precedence, deterministic seeds, and explicit evidence recording.
- [x] T014 [US3] Implement the isolated C++ runner/evidence handling in `tools/lib/validation.py` and `test` dispatch, with baseline flags, bounded execution, current build reports, and explicit `--record`.
- [x] T015 [US3] Create the minimal original KACTL compatibility prelude in `include/toolkit/kactl_prelude.hpp`; resolve/hash selected upstream dependency closures without copying the unlicensed contest template or modifying clones.
- [x] T016 [P] [US3] Write oracle/edge tests under `tests/cpp/` for C01-C03, C10-C13, C16, C27, C32, C38, C40: search, compression, sums, BFS/0-1 BFS/Dijkstra, Kruskal, topological sort, knapsack, binomial, Nim, and submasks.
- [x] T017 [P] [US3] Implement the 12 original fundamental helpers in `include/toolkit/` and corresponding `examples/<entry-id>.cpp`, satisfying T016 and documenting precise numeric/precondition behavior.
- [x] T018 [P] [US3] Write `tests/cpp/<entry-id>_test.cpp` oracles/edge cases for C04-C09: ACL DSU/Fenwick/segtree/lazy segtree and KACTL RMQ/rollback DSU.
- [x] T019 [P] [US3] Integrate C04-C09 into `examples/` with actual include roots/prelude/dependencies and explicit range/mapping/rollback contracts, satisfying T018.
- [x] T020 [P] [US3] Write `tests/cpp/<entry-id>_test.cpp` for C14-C15 and C17-C20: SCC, 2-SAT, LCA, max flow, min-cost flow, and bipartite matching, using reachability/assignment/parent/cut/flow/matching oracles.
- [x] T021 [P] [US3] Integrate C14-C15 and C17-C20 in `examples/` with recursion, residual mutation, cost/capacity, and graph preconditions explicit, satisfying T020.
- [x] T022 [P] [US3] Write `tests/cpp/<entry-id>_test.cpp` for C21-C26: prefix function, Z, suffix array, LCP, Aho-Corasick, and LIS, including alphabet, emptiness, duplicates, and reconstruction cases.
- [x] T023 [P] [US3] Integrate C21-C26 in `examples/`, preserving upstream semantics and declared valid domains, satisfying T022.
- [x] T024 [P] [US3] Write `tests/cpp/<entry-id>_test.cpp` for C28-C31 and C33-C35: sieve, modint, CRT, floor sums, linear systems, modular and integer convolution, with bounds/tolerances and independent small oracles.
- [x] T025 [P] [US3] Integrate C28-C31 and C33-C35 in `examples/`, keeping C++17, upstream permissions, full dependencies, capacity/overflow/conditioning contracts, satisfying T024.
- [x] T026 [P] [US3] Write `tests/cpp/<entry-id>_test.cpp` for C36-C37 and C39: hull, signed polygon area, and Simpson integration, using geometric/analytic oracles and degenerate/refinement cases.
- [x] T027 [P] [US3] Integrate C36-C37 and C39 in `examples/` with Point/prelude dependencies and valid numerical domains, satisfying T026.
- [x] T028 [US3] Merge all 40 per-entry contracts, dependencies, example/test IDs, and copy notices into `catalog/algorithms.json`; document compatibility/global-name/stack constraints in `docs/portability.md`.
- [x] T029 [US3] Run all 40 baseline examples/oracle suites offline, fix coupled integration failures, and explicitly record content-matched evidence in `catalog/validation.json`; do not reduce the required set silently.

**Independent Test**: Each family is independently compiled/tested against its
source pin. T029 requires all 40 to pass; upstream test reputation does not count.

**Parallel rule**: T016/T018/T020/T022/T024/T026 can proceed after T014-T015 and
source readiness. Each following integration task depends on its own tests.
Families own disjoint entry files; shared catalog/runner/prelude changes have
one owner and are merged through T028.

## Phase 5: User Story 4 - Configurable Contest Notebook (Priority: P2)

- [x] T030 [US4] Write `tests/python/test_notebook.py` for ordered selection, dependency/notice inclusion, copy blocking, unknown IDs, tested-only defaults, explicit study references, deterministic output, and offline links.
- [x] T031 [US4] Add `notebook/profiles/core.json` and the exact 12-entry `compact.json`; implement notebook generation in `tools/lib/render.py` with deduplicated dependencies/notices and readable print styles, satisfying T030.
- [x] T032 [US4] Write `docs/contest-workflow.md` and `docs/coverage.md` with practical preparation/pitfalls, individual readiness/gap counts, source/article distinctions, and contest-rule reminders; include relevant notes in generated notebooks.

**Independent Test**: Fixture/custom selections and the completed core profile
produce exact ordered offline output without dropped prerequisites/notices.

## Phase 6: User Story 5 - Reproducible Maintenance (Priority: P2)

- [x] T033 [US5] Replace `README.md` with actual entry-point commands; write `docs/usage.md` and `docs/testing.md` for prerequisites, examples, diagnosis, explicit pin/evidence updates, optional profiles, and no-network use.
- [x] T034 [US5] Add `tests/python/test_acceptance.py` for source/catalog/evidence/index/notebook integration, stale evidence, deterministic output, and network-disabled normal operations using isolated fixtures.
- [x] T035 [US5] Execute the complete `quickstart.md` flow from a separate clean temporary installation, including nine exact acquisitions, 40 baseline entries, offline browsing/links, both profiles, and no hidden dependencies; preserve evidence and actionable failures. With uncommitted implementation files, use and identify a fresh exported-working-tree snapshot rather than committed `HEAD`.

## Phase 7: Convergence and Handoff

- [x] T036 Reconcile `specs/001-comprehensive-icpc-toolkit/spec.md` acceptance criteria with actual outputs, `catalog/validation.json`, and `docs/coverage.md`; report exact source/integration/test/gap counts and blockers without pushing or publishing.

## Dependencies and Execution Order

| Task(s) | Depends on |
|---------|------------|
| T002 | T001 |
| T003 | T002 |
| T004-T005 | T003 |
| T006 | T004, T005 |
| T007-T008 | T006; T008 also needs T007 observations |
| T009 | T003 |
| T010 | T004, T007, T009 |
| T011 | T009, T010 |
| T012 | T011 |
| T013 | T003 |
| T014 | T013 |
| T015 | T007, T014 |
| Family test tasks T016/T018/T020/T022/T024/T026 | T015 |
| Family integration tasks T017/T019/T021/T023/T025/T027 | Their immediately preceding family test task |
| T028 | All six family integration tasks, T010 |
| T029 | T028 |
| T030 | T003, T011, T014 |
| T031 | T030, T012 |
| T032 | T010, T031 |
| T033 | T008, T012, T029, T031, T032 |
| T034 | T033 |
| T035 | T034 |
| T036 | T035 |

## Requirement and Acceptance Traceability

| Requirements | Primary tasks | Acceptance |
|--------------|---------------|------------|
| FR-001-FR-004, FR-018 | T004-T008 | SC-001, SC-006 |
| FR-005-FR-007, FR-015 | T009-T012, T032 | SC-002, SC-004 |
| FR-008-FR-012 | T013-T029 | SC-003, SC-009 |
| FR-013-FR-014 | T004, T015, T028, T030-T031 | SC-005, SC-007 |
| FR-016-FR-017 | T033-T035 | SC-008 |
| FR-019-FR-020 | All phases, T036 | SC-010 and worktree/no-publication constraints |

## Implementation Strategy

US1 is the first useful source-shelf increment, not complete toolkit delivery.
The full acceptance set includes US2-US5 and all 40 core entries. Parallel work
is limited to disjoint files after prerequisites; no separate repositories or
branches are needed. Stop for a concrete licensing/portability/correctness
blocker rather than substituting unverified code or weakening completion claims.
