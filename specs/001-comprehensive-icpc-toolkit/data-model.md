# Data Model

All persisted machine-readable data uses versioned UTF-8 JSON. Python standard
library validation is sufficient; no external schema package is required.
Reject unknown schema versions, duplicate IDs, invalid paths, dangling
references, malformed hashes, and contradictory states with explicit errors.

## Source Lock: `sources/lock.json`

Root: `schema_version`, `sources`.

Each source has:

- `id`: unique lowercase kebab-case.
- `name`, `repository_url`, `kind`: reusable-library, educational-articles,
  mixed-notebook-archive, or external-study-reference.
- `revision`: full 40-character Git SHA for enabled repository sources.
- `acquire`: explicit boolean; `inclusion_reason` explains both decisions.
- `attribution`: status (`verified`, `unverified`, `not-applicable`), named
  author/organization, evidence URLs, and observation date.
- `availability`: observed public/unavailable/unverified state, date, notes.
- `license`: declared SPDX value or `NOASSERTION`, evidence paths/URLs,
  repository observations, file exceptions, and copying restrictions.
- `portability_notes`, `submodules`, `managed_dependencies`.

Production acquisition accepts canonical HTTPS GitHub repository URLs only;
no embedded credentials, shell fragments, alternate transports, or arbitrary
destination paths. Destination is derived as `upstream/<id>/<revision>`.
Disabled sources can have a null repository/revision and external study links.
`ksan` is absent from acquisition candidates and documented as excluded.

## Algorithm Catalog: `catalog/algorithms.json`

Root: `schema_version`, `entries`.

Each entry has:

- `id`, `name`, `aliases`, `category`, `subcategory`, `tags`.
- `priority`: core or advanced; independent of implementation/validation.
- `implementation`: missing, reference-only, or implemented.
- `reason`: required for missing or blocked entries.
- `references`: source ID, pinned file path, code/article kind, source URL,
  file-level permission observations.
- `integration`: optional local helper/example path, include roots, explicit
  transitive source/local dependencies, compiler profile, test IDs.
- `contract`: purpose, supported domain, invalid-input policy, indexing,
  input/output, time/space complexity, bounds, mutation, global state,
  recursion/stack, and limitations.
- `copy_policy`: permitted/blocked with license evidence and notices.

Source references resolve through the lock. All local paths are repository-
relative, normalized, and cannot escape their allowed roots through traversal
or symlinks. Upstream identity and code authorship are different fields.
Original implementations identify this repository instead of inventing an
upstream source SHA. Their relevant content hashes are validation inputs.

## Evidence: `catalog/validation.json` and `build/validation.json`

Per entry/profile:

- Result: not-run, passed, failed, or stale.
- Check kinds: standalone compilation, edge cases, differential/exhaustive,
  analytic checks, optional sanitizers.
- Compiler executable/version, language standard, flags, platform.
- Test IDs, seeds, case counts, timeout, and oracle description.
- Fingerprint of relevant catalog contract, upstream pin/files, transitive
  dependencies, local helper/example/test code, and runner profile.
- Failure details and reproducing seed/input when applicable.

The committed catalog file records reviewed baseline evidence after explicit
`test --record`; build output represents current runs. A current failure or
fingerprint mismatch overrides a historical pass. Reports are not signatures
or security attestations. Runtime timestamps belong in separate logs and are
excluded from deterministic generated document content.

## Notebook Profile: `notebook/profiles/*.json`

Fields: `schema_version`, `name`, `description`, ordered unique `entries`,
`include_study_references` (default false), and `print_options`.

Unknown IDs, blocked copying, missing dependencies, or absent current test
evidence in ready-to-use profiles are errors. An opt-in study profile may
include references only with prominent untested/type/permission labeling.
Selection never implies all entries form one compilable translation unit.
No automatic page budget truncation is permitted.

## State Transitions

| Trigger | State effect |
|---------|--------------|
| A credible source path is verified | missing may become reference-only |
| Real adapter/example and dependencies are added | reference-only may become implemented |
| Local matching oracle/edge checks pass | implemented can display tested for that profile |
| Source/helper/contract/test changes | previous evidence becomes stale |
| Current run fails | failed overrides historical pass |
| Source becomes unavailable | acquisition/local-availability state changes; implementation evidence is not silently erased or assumed runnable |
| License ambiguity is discovered | copying/inclusion is blocked until resolved |

Never transition from source acquisition or upstream reputation directly to
locally tested. Counts are per stable entry, not number of source files.
