# ICPC Preparation Toolkit Constitution

## Core Principles

### I. Evidence Before Confidence

Track acquisition, implementation, and validation separately. An upstream file
is not a locally tested integration. Every ready-to-use entry needs a precise
source, dependency closure, contract, example, and matching local evidence.
Never infer test success from an author's reputation or an upstream badge.

### II. Preserve Provenance and Permissions

Keep upstream checkouts unmodified. Preserve authors, revisions, notices, and
per-file license exceptions. Public visibility is not a reuse license.
Unresolved permissions block copied curated/notebook content. Do not relicense
third-party work or assign the user's original code a license without consent.

### III. Reproducible and Non-Destructive Acquisition

Acquire only approved sources at full commit IDs. Never reset existing
checkouts, discard changes, advance branch tips, run upstream scripts, or
initialize arbitrary submodules implicitly. Reject unsafe paths and origin/
revision mismatches. Fail explicitly instead of silently substituting sources.

### IV. Bounded Integrations and Honest Breadth

Prefer established implementations. Small original fundamentals require local
oracle tests; obscure unverified implementations are not a substitute for
missing references. Enumerate algorithms individually. Core/advanced expresses
study priority, not category-wide implementation or correctness.

### V. Explicit Portability and Contest Constraints

Default to C++17 on Linux/GCC. Record newer standards, GNU extensions, SIMD,
numeric bounds, indexing, recursion, and global-state limitations per entry.
Do not enable AVX2 or native CPU optimization by default. Notebook selection is
configurable; page limits and electronic-reference permission are not assumed.

### VI. Offline First and Minimal Tooling

After explicit acquisition, searching, browsing, notebook generation, and tests
must work offline. Use Git, Python standard library tooling, and a C++ compiler;
do not require hosted assets, TeX, or a server. Ignore clones/build outputs to
keep the main repository small.

## Scope and Ownership

Only the dedicated worktree in `NatiYoni/Templates-Competitive-Programming` is
in scope. Preserve existing work. Exclude `ksan`. Treat unresolved tourist/
jiangly sources honestly. Authoring Spec Kit documents is separate from
approval to implement the toolkit. No push, publication, or PR is authorized.

## Development Workflow and Quality Gates

Use the official Spec Kit constitution/specification/plan/tasks workflow.
Require isolated C++ compilation, deterministic oracle comparisons, explicit
preconditions, stale-evidence detection, offline links, and a clean-checkout
walkthrough. Source/dependency/adapter/test changes invalidate old evidence.

## Governance

This constitution was ratified with the implementation plan on 2026-09-12.
Scope, permissions, and portability changes require corresponding
specification and acceptance-criteria updates.

**Version**: 1.0.0 | **Ratified**: 2026-09-12 | **Last Amended**: 2026-09-12
