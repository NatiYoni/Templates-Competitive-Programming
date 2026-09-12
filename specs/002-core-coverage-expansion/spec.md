# Feature Specification: Core Coverage Expansion

**Feature ID**: `002-core-coverage-expansion`
**Branch**: `natiyoni-comprehensive-icpc-toolkit`
**Created**: 2026-09-12
**Status**: Implemented; SC-001 through SC-007 satisfied for the recorded
content-bound snapshot. See [acceptance evidence](../../docs/testing.md#expansion-delivery-checkpoint).

## User Scenarios & Testing

### User Story 1 - Reuse 36 additional integrations (Priority: P1)

As an ICPC contestant, I can use the exact additions in [coverage.md](coverage.md)
with GNU C++17, concrete contracts, real dependencies, and independent examples
and oracle cases. The original 40 remain reproducible.

Acceptance: every selected ID is implemented and passes local baseline evidence.
Combined lowlink outputs count as one integration, not three wrappers. Worked
DP techniques identify their actual problem/recurrence; no universal DP solver
is implied. Randomized algorithms disclose collision/runtime limitations.

### User Story 2 - Understand every old gap (Priority: P1)

As a learner, I can inspect an individual disposition for all 43 v1 missing IDs.
A disposition is an implemented scoped algorithm, a concrete pinned reference,
a substantive original educational reference, or a documented residual gap.

Acceptance: every old ID appears exactly once in the audit. Educational content
has honest local authorship and remains reference-only. No source SHA or tested
badge is invented for prose. Missing count is an outcome, not a zero-gap target.

### User Story 3 - Keep notebook and validation scopes explicit (Priority: P1)

As a maintainer, I can reproduce the original core40/compact12, the new
expansion36, or the combined expanded76 selection without taxonomy heuristics.

Acceptance: original profiles remain unchanged; an explicit CLI selection
profile selects test IDs independently of compiler profiles. Offline indexes
and ordered notebooks retain dependencies/notices and reject stale evidence.

### Edge Cases

Parallel/self edges, disconnected graphs, negative cycles and unreachable
vertices; empty ranges/strings/trees where supported; persistent version
lifetimes; inclusive versus half-open ranges; DP optimization assumptions;
unsigned 64-bit arithmetic and bounded products; geometric boundary cases;
randomized termination/collisions; original-document path/symlink containment;
invalid profiles, stale evidence, missing dependencies and licenses.

## Requirements

- **FR-001**: Add exactly the 36 integration IDs in coverage.md, without adding
  duplicate catalog IDs or removing original entries.
- **FR-002**: Preserve v1 source pins, checkouts, core40 and compact12 profiles,
  historical feature001 records, and pre-existing changes.
- **FR-003**: Each new integration has an example, actual prerequisite closure,
  complete contract, and independent seeded oracle/edge evidence.
- **FR-004**: Prefer compatible permissive pinned implementations. Clearly
  identify independently authored code and do not relicense third-party work.
- **FR-005**: Audit all 43 original gaps individually, including residual ones.
- **FR-006**: Represent original educational references explicitly; validate
  their paths/authorship and never treat them as tested implementations.
- **FR-007**: Keep `test --all-core` selecting the original40. Add
  `test --selection-profile PATH` separately from compiler `--profile`.
- **FR-008**: Add expansion36 and expanded76 notebook/test selections.
- **FR-009**: Regenerate live counts, documentation, offline index and selected
  notebooks; preserve permissions, dependencies and deterministic output.
- **FR-010**: Reproduce the complete expanded workflow from a fresh working-tree
  export; no hidden source symlinks, packages, network-dependent normal commands,
  commits, publication, main-checkout changes or unrelated repository work.

## Success Criteria

- **SC-001**: Exactly 76 implemented catalog entries: original40 plus new36.
- **SC-002**: All76 have current passing GNU C++17 baseline records, with at
  least 200 appropriate seeded/exhaustive cases for every new integration.
- **SC-003**: All43 original missing targets have evidence-based dispositions;
  final implementation/reference/missing counts sum to 265.
- **SC-004**: Original core/compact selections are byte-preserved. Expanded
  test/profile selection has regression coverage for exact IDs and bad inputs.
- **SC-005**: Original prose references remain untested, have validated local
  provenance, and work through search, index and explicit study notebooks.
- **SC-006**: Generated local links, actual file-based browser search, notebook
  dependency/permission checks and same-input determinism pass.
- **SC-007**: A fresh snapshot independently acquires the nine pins and passes
  the expanded workflow; evidence and observed limitations are retained.

## Non-Goals

Universal DP templates, collision-free probabilistic hashes, bounded worst-case
Pollard rho, every ICPC algorithm, zero missing by relabeling, new source-identity
hunts, C++23 promotion, upstream modifications, a fixed notebook page cap,
one-file amalgamation, or committing/pushing/publishing this work.
