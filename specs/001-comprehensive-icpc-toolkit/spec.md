# Feature Specification: Comprehensive ICPC Preparation Toolkit

**Feature Branch**: `natiyoni-comprehensive-icpc-toolkit`

**Feature ID**: `001-comprehensive-icpc-toolkit`

**Created**: 2026-09-12

**Status**: Implemented and acceptance-complete for the bounded C++17 core;
approved on 2026-09-12. See [acceptance results](checklists/requirements.md).

**Input**: Build a dependable C++ ICPC preparation collection in
`NatiYoni/Templates-Competitive-Programming`, combining the requested public
libraries with attributable tested examples, concrete coverage/gap tracking,
and an offline configurable contest notebook. Prepare official GitHub Spec Kit
planning documents first. Exclude `ksan`.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Acquire an attributable source shelf (Priority: P1)

As a contestant, I can reproduce approved upstream repositories locally and
identify their authors, revisions, and restrictions without damaging files.

**Why this priority**: Reliable acquisition underpins study and curation.

**Independent Test**: Acquire into an empty temporary checkout, compare all
origins/commits with the lock, then repeat offline.

**Acceptance Scenarios**:

1. **Given** a fresh checkout and network, **When** I sync all enabled sources,
   **Then** nine approved repositories exist at exact pins with notices intact.
2. **Given** a modified or conflicting destination, **When** I sync, **Then**
   acquisition fails with a source-specific diagnostic and preserves its files.
3. **Given** unresolved named sources, **When** I inspect the manifest, **Then**
   identity, availability, permissions, and inclusion decisions are separate.
4. **Given** acquired pins, **When** I sync offline, **Then** existing clones
   are verified without network access or changes.

### User Story 2 - Search concrete algorithms and gaps (Priority: P1)

As a contestant, I can search by algorithm/technique, browse core/advanced
topics, and distinguish implementations, references, and missing coverage.

**Why this priority**: Archive size is not usable or verified coverage.

**Independent Test**: Generate/search a fixture index containing all statuses;
inspect filters, counts, local links, and missing/stale conditions.

**Acceptance Scenarios**:

1. **Given** the catalog, **When** I search "persistent segment tree", "2-SAT",
   or "centroid decomposition", **Then** I find individual entries with aliases,
   source locations, constraints, and honest status.
2. **Given** no matching local evidence, **When** I browse an upstream header,
   **Then** it is not labeled locally tested.
3. **Given** sources and no network, **When** I open the HTML directly or the
   Markdown index, **Then** navigation, search, and local source links work.
4. **Given** stale evidence or a missing source/file, **When** I generate the
   index, **Then** the problem is visible rather than rendered as success.

### User Story 3 - Reuse a locally tested core (Priority: P1)

As a contestant, I can compile and use the 40 core entries in
[coverage.md](coverage.md) with actual dependencies and explicit contracts.

**Why this priority**: Local integration evidence makes snippets dependable.

**Independent Test**: Build each example independently and run its specified
oracle/edge cases with recorded seeds and compiler profile.

**Acceptance Scenarios**:

1. **Given** a core entry, **When** I follow its example, **Then** dependencies,
   preconditions, expected output, and compile instructions are sufficient.
2. **Given** valid small randomized inputs, **When** I run its tests, **Then**
   results agree with an independent oracle; failures reproduce with seed/input.
3. **Given** changed source, dependencies, adapter, or tests, **When** old
   evidence is read, **Then** it is stale until affected tests pass again.
4. **Given** advanced/platform-specific code without integration evidence,
   **When** it is indexed, **Then** it remains reference-only.

### User Story 4 - Prepare an offline notebook (Priority: P2)

As a contestant, I can select a compact/custom notebook containing snippets,
prerequisites, attribution, caveats, and preparation checklists.

**Why this priority**: Actual contest page/reference rules vary.

**Independent Test**: Generate compact/custom notebooks, verify selections and
dependency inclusion, and open/print offline.

**Acceptance Scenarios**:

1. **Given** ordered entry IDs, **When** I generate a notebook, **Then** it
   contains precisely those entries plus required dependencies/notices.
2. **Given** identical inputs/evidence, **When** I regenerate, **Then**
   document contents and ordering are deterministic.
3. **Given** missing/blocked/untested selected code, **When** I request a
   ready-to-use profile, **Then** it fails instead of dropping or mislabeling it.
4. **Given** generated HTML, **When** I print to PDF, **Then** code and notices
   remain readable; exact pagination is not universally guaranteed.

### User Story 5 - Maintain and reproduce the toolkit (Priority: P2)

As a maintainer, I can diagnose prerequisites, review pin changes, rerun
affected tests, and reproduce the toolkit without undocumented machine state.

**Why this priority**: Trust must survive future updates.

**Independent Test**: Follow the clean-checkout quickstart and deliberate
failure/stale-evidence scenarios.

**Acceptance Scenarios**:

1. **Given** documented prerequisites, **When** I follow the quickstart,
   **Then** acquisition, examples, index, and notebook reproduce without TeX,
   upstream test frameworks, or unlisted global packages.
2. **Given** a changed pin, **When** it is reviewed, **Then** permissions,
   dependencies, portability, and evidence are reviewed before restoring badges.
3. **Given** a missing tool or failed command, **When** I invoke the toolkit,
   **Then** it exits nonzero with the actionable cause.

### Edge Cases

- Empty/singleton inputs; disconnected graphs; duplicate edges/patterns; cycles;
  negative weights where unsupported; inconsistent congruences; noninvertible
  values; ties; collinearity; singular/ill-conditioned matrices; overflow.
- Invalid ranges; empty patterns; alphabet limits; recursive chains; global
  state between runs; sentinels and bounded capacities.
- Interrupted fetches; unfetchable pins; dirty/wrong-origin checkouts;
  unexpected gitlinks; escaping paths/symlinks; concurrent writers.
- Paths with spaces; inherited license exceptions; special text characters;
  stale reports; `file://` restrictions; missing newer-standard/SIMD support.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Acquire the nine enabled repositories in [research.md](research.md);
  retain disabled tourist/jiangly records and exclude `ksan`.
- **FR-002**: Record canonical URL, full commit, classification, identity
  evidence, availability, permissions/exceptions, and inclusion decision.
- **FR-003**: Preserve unmodified attributable clones; commit manifests and
  curation, not giant nested repositories/history.
- **FR-004**: Acquisition MUST be pinned, idempotent, non-destructive, and
  explicit about failures; no implicit revision changes.
- **FR-005**: Every named algorithm/technique in the coverage inventory MUST
  have an individual stable catalog entry with core/advanced priority.
- **FR-006**: Distinguish `missing`, `reference-only`, and `implemented`;
  independent evidence is `not-run`, `passed`, `failed`, or `stale`.
  Display `tested` only for current content-matched local passing evidence.
- **FR-007**: Supply offline HTML search/filtering and a Markdown index with
  working relative local links.
- **FR-008**: Integrate the 40 named core entries with source/file/pin
  attribution, true dependencies, and independent compiling examples.
- **FR-009**: Document preconditions, complexity, indexing, numeric limits,
  mutations, global state, recursion, and known limitations per integration.
- **FR-010**: Keep C++17 as the default; label C++20/23, GNU/PBDS, AVX2, and
  other non-baseline requirements without silently enabling them.
- **FR-011**: Provide local oracle/edge-case tests and reproducible evidence
  for each core entry; upstream assertions are not local test results.
- **FR-012**: Invalidate evidence when source, dependencies, adapter, or tests
  change; distinguish compile-only and algorithm-validation evidence.
- **FR-013**: Generate deterministic configurable HTML/Markdown notebooks
  with dependencies/notices and an offline print-to-PDF path.
- **FR-014**: Preserve permissions and exceptions; block unresolved-license
  snippets from copied curation/notebooks.
- **FR-015**: Cover all enumerated study domains with explicit reference-only/
  missing states; do not inflate category-wide completeness.
- **FR-016**: During implementation, replace the minimal README with useful
  usage/setup/maintenance instructions and document remaining gaps.
- **FR-017**: Reproduce from a clean checkout; after acquisition, local
  operations MUST NOT require a network.
- **FR-018**: Never implicitly execute upstream setup/tests, shell-escape
  notebook commands, or recursive submodule acquisition.
- **FR-019**: Report unresolved sources, integration failures, and coverage
  gaps without silently reducing the required core.
- **FR-020**: Work only in this destination worktree; no main checkout,
  `Sydekse/suqbot`, push, or publication changes.

### Key Entities

- **Source**: Identity, provenance, pin, type, availability, permissions,
  compiler requirements, and acquisition decision.
- **Algorithm entry**: Stable ID, aliases, taxonomy/priority, implementation
  state, source/dependency references, contract, and example/test IDs.
- **Validation evidence**: Seed/case profile, compiler/flags, content
  fingerprints, result, and reproduction details.
- **Notebook profile**: Ordered selection and explicit policy for labeled
  untested study references, dependencies, and printing.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Nine enabled repositories reproduce at exact pins; repeated
  offline acquisition changes no checkout.
- **SC-002**: Every inventory name has an individual catalog entry; category
  labels are never substituted for algorithm-level evidence.
- **SC-003**: All 40 core entries have examples, contracts, resolved
  dependencies/permissions, and current passing local evidence.
- **SC-004**: All generated local/internal links resolve after full acquisition;
  index/notebook resources work without a web service.
- **SC-005**: Identical inputs produce byte-identical documents; separate
  nondeterministic run logs are excluded.
- **SC-006**: Required acquisition failure cases preserve existing files,
  return nonzero, and identify the source/path.
- **SC-007**: Compact/custom selection includes only selected entries and
  necessary dependencies, in the requested order.
- **SC-008**: The clean-checkout walkthrough succeeds using only documented
  baseline prerequisites.
- **SC-009**: Changed source/dependencies/tests cannot retain a current
  `tested` label using old evidence.
- **SC-010**: Final output reports exact acquired/integrated/tested counts,
  exclusions, unavailability, and reference-only/missing gaps.

## Assumptions

- Approved architectural defaults are C++17, Python standard library tools,
  ignored pinned clones, and HTML/Markdown with browser print-to-PDF.
- Network is available for initial acquisition, not required afterward.
- Current ecnerwala and CP-Algorithms auxiliary sources require C++23; they
  remain references unless separately integrated under an explicit profile.
- Contest reference rules and page limits are user/contest-specific.
- No repository-wide license is selected for the user's original code.

## Non-Goals

No every-algorithm/every-problem guarantee, wholesale upstream certification,
unlimited competitor hunt, disguised solution archives, one-file amalgamation
of all libraries, judge submission service, automatic native tuning, upstream
script execution, or fixed PDF page limit.
