# Requirements Quality Checklist

**Purpose**: Record design completeness and the bounded implementation's
acceptance results. These do not certify whole upstream libraries or every
algorithm in the study inventory.

| Review item | Planning result |
|-------------|-----------------|
| Destination/worktree and preservation constraints explicit | Defined |
| Official Spec Kit scaffold/templates used | Specify CLI 1.0.1 initialized; completed documents materialized |
| Prioritized independent user stories and acceptance scenarios | Five stories defined |
| Source list finite, full pins and provenance evidence recorded | Nine enabled candidates; tourist/jiangly disabled; ksan excluded |
| License scope and exceptions distinguished from API metadata | Defined; KACTL/Benq/Suisen/ACL/ecnerwala exceptions recorded |
| Concrete algorithms rather than category completeness | Forty named integrations plus finite individual-reference inventory |
| Unavailable, unverified, and reference-only material honest | Separate status/decision fields and no substitute claims |
| Clone storage, updates, and safe failure behavior | Content-addressed ignored clones; no reset/tip fallback |
| Compiler and platform constraints | C++17 baseline, current C++23 sources and SIMD/GNU caveats explicit |
| Dependency and precondition documentation | Required per integration with representative contracts |
| Offline local links/search and notebook selection | Defined with file-based HTML and deterministic profiles |
| Test evidence and staleness | Local oracles, seeds, input fingerprints, failed-rerun precedence |
| Measurable completion criteria | SC-001 through SC-010 |
| Tasks trace requirements and have concrete file paths/dependencies | T001-T036 |
| Unknown contest rules handled without arbitrary defaults | Custom profiles; user checks contest rules |
| Honest non-goals and bounded implementation | Defined; no universal algorithm/problem guarantee |
| Implementation gate respected | Document-only stage completed first; subsequent explicit C++17-core approval authorized implementation |

## Implementation Acceptance

All T001-T036 tasks are complete. The primary reproducible evidence is in
[`catalog/validation.json`](../../../catalog/validation.json); detailed
environment and print observations are in
[`docs/testing.md`](../../../docs/testing.md#initial-delivery-checkpoint).

| Criterion | Observed result |
|-----------|-----------------|
| SC-001: exact attributable source pins | Nine repositories acquired independently in both installations; origin, HEAD, content, notices, and offline verification passed. |
| SC-002: individual coverage inventory | 265 catalog entries: 40 implemented, 182 reference-only, 43 explicitly missing; concrete topic/source counts are documented. |
| SC-003: complete core integration | All 40 standalone examples and oracle suites passed with real dependencies and contracts; 27,634 cases, seed 1729, minimum 262 per entry. |
| SC-004: local documents and navigation | HTML/Markdown links passed; actual file-based browser search/filters worked with local resources only and zero script exceptions. Both notebooks printed with all code/prose blocks retained. |
| SC-005: deterministic generated documents | All ten index/compact/core HTML, Markdown, CSS, and JavaScript artifacts were byte-identical across independent installations with matching inputs. |
| SC-006: safe acquisition failures | Existing acquisition fixtures passed for dirty/wrong-origin destinations, unavailable pins, fetch failure, traversal/symlinks, concurrency, and cleanup/preservation. |
| SC-007: ordered configurable notebook | Exact 12-entry compact and 40-entry core selections generated with required dependencies/notices; custom-selection and permission-blocking fixtures passed. |
| SC-008: reproducible fresh installation | A fresh exported-working-tree snapshot independently acquired sources and completed the documented workflow. This is not a claim that the uncommitted toolkit exists in committed HEAD. |
| SC-009: honest evidence invalidation | Stale/failure/dependency fixtures passed, including system-marked local headers, symlink aliases, and stale/undeclared upstream pin detection. |
| SC-010: bounded final report | Source, core, reference, missing, copy-blocked, and unresolved-source counts are explicit; no core entry was dropped to meet acceptance. |

The existing Python suite passed **73 tests**. Baseline algorithm evidence was
produced with GCC 15.2.0, `-std=c++17 -O2 -Wall -Wextra`, on Linux x86_64;
fresh-install evidence matched all 40 fingerprints.

Tourist/jiangly remain disabled because verification is unresolved; `ksan` is
excluded. Seven reference entries remain copy-blocked by unresolved
permissions. Current ecnerwala/CP-Algorithms auxiliary C++23 material and
platform-specific references are not certified as C++17 core integrations.
ETCPC's exact compiler and contest page/reference rules remain unverified.
Observed A4 PDFs were 57 pages for compact and 169 for core, not universal
page guarantees. No remaining implementation blocker is known within the
approved scope. No commit, push, pull request, or unrelated-checkout change
was made.
