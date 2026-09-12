# Exact Expansion Boundary

The baseline is 265 entries: 40 implemented, 182 reference-only, 43 missing.
These **36 existing IDs** become additional implementations, for 76 total.
Seven are old gaps; before other audit resolutions the counts would be
76 implemented, 153 reference-only, and 36 missing. The completed
[individual audit](../../docs/gap-audit.md) resolves31 more gaps as references:
the final inventory is **76 implemented, 184 reference-only, five missing**.
These are implementation counts, separate from current test-evidence status.

## Graph, Tree, and Flow (10)

| ID | Integration boundary |
|----|----------------------|
| biconnected-components | One undirected lowlink integration exposing bridges, articulation vertices and biconnected edge blocks; edge identity handles parallel edges. |
| bellman-ford | Directed signed shortest paths; distinguish unreachable, finite and negative-cycle-affected destinations. |
| floyd-warshall | All-pairs signed shortest paths with explicit unreachable/negative-cycle reachability semantics. |
| eulerian-walk | Edge-indexed Euler trail construction, supported directed/undirected domain explicit, disconnected-edge rejection. |
| heavy-light-decomposition | Tree path/subtree decomposition with explicit order/edge-versus-vertex conventions. |
| centroid-decomposition | Tree centroid hierarchy and a concrete independently checked use, not arbitrary centroid-query claims. |
| rerooting-dp | Associative rerooting framework or explicitly scoped all-roots aggregate, with operation-order and tree assumptions. |
| assignment-hungarian-algorithm | Rectangular minimum-cost bipartite assignment, feasible dimensions and signed cost bounds. |
| lower-bound-circulation | Integral directed feasible circulation with lower/upper bounds and actual recovered edge flows. |
| dag-shortest-paths | Signed DAG shortest paths, explicit cycle rejection and unreachable state. |

## Data Structures and DP (12)

| ID | Integration boundary |
|----|----------------------|
| persistent-segment-tree | Persistent point-update/range-sum versions; valid roots, memory and overflow contracts. |
| rollback-offline-dynamic-connectivity | Add/remove/query timeline with rollback DSU and edge multiplicity semantics. |
| monotone-queue | Sliding-window extrema with index expiry and duplicate semantics. |
| mos-algorithm | Offline static range-query ordering and a real distinct-count integration. |
| parallel-binary-search | Batched monotone earliest-prefix queries with reset/update/check lifecycle. |
| dynamic-li-chao-tree | Online line insertion and minimum queries on a bounded integer domain; exact wide evaluation. |
| digit-dp | Explicitly scoped digit-state counting framework/example with leading-zero, zero and bound semantics. |
| interval-dp | Worked optimal adjacent-merge interval DP with reconstruction or clear recurrence, not universal interval solving. |
| divide-and-conquer-dp-optimization | Layered partition recurrence with documented monotone-optimum preconditions and feasible states. |
| knuth-optimization | Optimal adjacent merges under nonnegative weights, justified Knuth conditions and cubic oracle. |
| sos-dp | Subset zeta/Mobius transforms with power-of-two dimensions and arithmetic bounds. |
| meet-in-the-middle-subset-optimization | Bounded-size subset-sum optimization with exact feasibility and numeric contracts. |

## Number Theory and Linear Algebra (7)

`deterministic-64-bit-miller-rabin`, `pollard-rho-factorization`,
`xor-linear-basis`, `matrix-exponentiation`, `extended-gcd`,
`discrete-logarithm`, `subtraction-games`.

Require correct unsigned 64-bit modular arithmetic, explicit randomized
factorization retry/failure behavior, rank/membership XOR semantics, actual
matrix arithmetic bounds, signed Bezout domains, non-coprime discrete-log
semantics or explicit supported restrictions, and finite normal-play
subtraction-game rules.

## Strings and Geometry (7)

`manacher-palindrome-radii`, `suffix-automaton`, `rolling-hash`,
`longest-common-subsequence`, `segment-intersection`, `point-in-polygon`,
`closest-pair`.

Require explicit alphabets/empty inputs, automaton lifetime, hash collision
caveats, LCS reconstruction semantics, exact geometric degeneracies/boundaries,
coordinate-product bounds and closest-pair behavior for fewer than two points.

## Gap Audit

Audit all43 v1 missing IDs once. Use exact pinned files when they actually
address the target; otherwise use a substantive original educational reference
with a problem/invariant/example/pitfalls section, or retain an explicit gap.
Record old and new states, paths, provenance and rationale in the follow-on
audit. General techniques remain educational, not generic tested solvers.
