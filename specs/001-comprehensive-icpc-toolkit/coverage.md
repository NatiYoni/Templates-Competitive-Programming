# Algorithm-Level Coverage and Initial Integration Boundary

## Meaning of This Inventory

This inventory defines the exact 40-entry implementation acceptance set and
the broader individually tracked study scope. The delivered catalog contains
**40 implemented integrations, 182 reference-only entries, and 43 explicit
gaps**. Current tested readiness is derived from content-matched evidence, not
from this design table. See [delivered coverage](../../docs/coverage.md) and
[local evidence](../../docs/testing.md). No whole category is called complete
on the strength of one example.

Source IDs and full revisions are in [research.md](research.md). `original`
means a small independently authored fundamental, not a disguised upstream copy.
The original implementation paths below now exist. Expanded complexity,
numeric bounds, mutation, stack/global state, and indexing contracts are
recorded per entry in `catalog/algorithms.json`.

## Initial Core: 40 Required Locally Tested Entries

| # | Stable ID / algorithm | Source or original path | Contract focus and independent validation |
|---|-----------------------|----------------------------------|-------------------------------------------|
| C01 | `boundary-search` / integer boundary binary search | original `include/toolkit/search.hpp` | Monotone predicate, half-open domain, no overflowing midpoint; compare every boundary in small domains and endpoints. |
| C02 | `coordinate-compression` | original `include/toolkit/compression.hpp` | Sort/deduplicate and rank mapping, empty/duplicate/signed inputs; compare to sorted unique oracle. |
| C03 | `prefix-sums` / one-dimensional prefix/range sums | original `include/toolkit/prefix_sums.hpp` | Zero-based half-open ranges, bounded signed sums; compare direct summation including empty ranges. |
| C04 | `dsu` | acl `atcoder/dsu.hpp` | Merge/same/size/groups, vertex bounds; compare explicit component labels. |
| C05 | `fenwick` / point-add range-sum | acl `atcoder/fenwicktree.hpp` | Zero-based updates, half-open sums, type limits; compare mutable array. |
| C06 | `segtree` / monoid range queries and boundary searches | acl `atcoder/segtree.hpp` | Identity/associativity and monotone predicates; compare array products and linear boundary searches. |
| C07 | `lazy-segtree` / range-affine range-sum example | acl `atcoder/lazysegtree.hpp`, `atcoder/modint.hpp` | Mapping/composition order, segment lengths, modular bounds; compare naive affine array updates. |
| C08 | `sparse-table` / static RMQ | kactl `content/data-structures/RMQ.h` | Nonempty query ranges, immutable input; compare direct minima. |
| C09 | `rollback-dsu` | kactl `content/data-structures/UnionFindRollback.h` | Snapshot/rollback semantics, no path compression; compare copied component state. |
| C10 | `bfs` / unweighted shortest paths | original `include/toolkit/bfs.hpp` | Unreachable sentinel, source and graph bounds; compare small all-pairs distances. |
| C11 | `zero-one-bfs` | original `include/toolkit/zero_one_bfs.hpp` | Edge costs exactly zero/one; compare independent small all-pairs shortest paths. |
| C12 | `dijkstra` | original `include/toolkit/dijkstra.hpp` | Nonnegative costs, bounded additions, unreachable states; compare Bellman-Ford oracle. |
| C13 | `kruskal` / minimum spanning forest | original `include/toolkit/kruskal.hpp` plus ACL DSU | Undirected edges, disconnected/parallel/negative edges; enumerate small forests for optimum. |
| C14 | `scc` | acl `atcoder/scc.hpp` | Directed graph, component ordering contract; compare mutual reachability. |
| C15 | `two-sat` | acl `atcoder/twosat.hpp` | Literal/assignment convention and unsatisfiability; enumerate assignments. |
| C16 | `topological-sort` / Kahn's algorithm | original `include/toolkit/topological_sort.hpp` | Directed cycles reported explicitly, deterministic tie behavior documented; compare exhaustive DAG orders/cycle oracle. |
| C17 | `lca` | kactl `content/graph/LCA.h` plus `RMQ.h` | Valid nonempty tree, root zero, recursion depth; compare parent climbing, singleton/star/chain cases. |
| C18 | `max-flow` | acl `atcoder/maxflow.hpp` | Capacity type/limit, residual mutation, min-cut semantics; enumerate small cuts. |
| C19 | `min-cost-flow` | acl `atcoder/mincostflow.hpp` | Nonnegative original costs per upstream contract, capacity/cost overflow, call semantics; enumerate small feasible flows. |
| C20 | `bipartite-matching` / Hopcroft-Karp | kactl `content/graph/HopcroftKarp.h` | Left adjacency/right match representation; enumerate small matchings including empty partitions. |
| C21 | `prefix-function` / KMP prefix function and search usage | kactl `content/strings/KMP.h` | Empty-string/pattern policy and matching offsets; compare naive prefixes and substring search. |
| C22 | `z-function` | acl `atcoder/string.hpp` | Empty input and first-element convention; compare direct prefix matching. |
| C23 | `suffix-array` | acl `atcoder/string.hpp` | Alphabet bounds/overloads and empty strings; compare sorting explicit suffixes. |
| C24 | `lcp-array` | acl `atcoder/string.hpp` | Valid suffix array and nonempty upstream preconditions; compare direct adjacent suffix LCPs. |
| C25 | `aho-corasick` | kactl `content/strings/AhoCorasick.h` | Default alphabet `A`-`Z`, nonempty patterns, duplicates, match-index semantics; compare naive multi-pattern matching. |
| C26 | `lis` / strictly increasing subsequence reconstruction | kactl `content/various/LIS.h` | Strict versus nondecreasing, duplicates, returned indices; enumerate subsequences. |
| C27 | `zero-one-knapsack` | original `include/toolkit/knapsack.hpp` | Nonnegative weights/capacity, bounded values, zero-weight items used once; enumerate subsets. |
| C28 | `prime-sieve` | kactl `content/number-theory/Eratosthenes.h` | Exclusive limit within fixed `MAX_PR`, global bitset, repeat calls; compare trial division. |
| C29 | `modular-arithmetic` / static and dynamic modular integers | acl `atcoder/modint.hpp` | Positive modulus, normalization, valid inverses, dynamic-modulus lifetime; compare small integer arithmetic/gcd. |
| C30 | `crt` / generalized Chinese remainder theorem | acl `atcoder/math.hpp` | Positive moduli, inconsistent systems, bounded LCM; enumerate residues for small systems. |
| C31 | `floor-sum` | acl `atcoder/math.hpp` | Exact upstream argument/type bounds, negative coefficient normalization; compare direct small sums. |
| C32 | `binomial-coefficients` / prime-modulus factorial tables | original `include/toolkit/binomial.hpp` plus ACL modint | Known prime, precomputation below modulus, invalid-index policy; compare Pascal's triangle. |
| C33 | `gaussian-elimination` / real linear system solving | kactl `content/numerical/SolveLinear.h` | Rank/inconsistency, input mutation, tolerance/conditioning; constructed systems and small rational reference cases. |
| C34 | `modular-convolution` / NTT-backed polynomial product | acl `atcoder/convolution.hpp`, `atcoder/modint.hpp` | Supported NTT prime/length, empty vectors; compare quadratic multiplication. |
| C35 | `integer-convolution` / exact signed 64-bit product | acl `atcoder/convolution.hpp` | Document upstream transform and result bounds; compare bounded quadratic multiplication. |
| C36 | `convex-hull` / planar monotone-chain hull | kactl `content/geometry/ConvexHull.h`, `Point.h` | Integer products within range, duplicates/collinear policy/orientation; compare independent gift wrapping and hull containment. |
| C37 | `polygon-area` / twice signed area | kactl `content/geometry/PolygonArea.h`, `Point.h` | Nonempty ordered polygon, bounded accumulation, orientation; compare shoelace/known rectangles and triangulations. |
| C38 | `nim` / normal-play Nim winner and move | original `include/toolkit/nim.hpp` | Nonnegative heaps, normal play only; compare exhaustive game-state recursion. |
| C39 | `simpson-integration` | kactl `content/numerical/Integrate.h` | Positive subdivision count, finite continuous integrand, approximation caveat; analytic polynomial integrals and refinement checks. |
| C40 | `submask-enumeration` | original `include/toolkit/submasks.hpp` | Unsigned bounded masks, empty submask once, defined shift widths; compare filtered subset enumeration. |

Selected KACTL headers have explicit CC0 declarations except ConvexHull
(Unlicense). Source/dependency notices stay intact. The prelude is independently
authored; unused upstream macros/global optimizations are not imported.

## Broader Inventory: Individual References or Explicit Gaps

Each semicolon-separated name below is a distinct target catalog entry unless
already represented in the core table. Split compound labels into their named
algorithms when their assumptions or source/test evidence differ. References
do not inherit a locally-tested state.

| Domain | Additional core study/reference targets | Advanced reference targets |
|--------|-----------------------------------------|----------------------------|
| Fundamentals, search, sorting | Comparator/strict-weak-order rules; stable sorting; two pointers; sliding window; sweep-line events; ternary search; meet-in-the-middle; bit operations; recursion/backtracking | Parallel binary search; fractional binary search; branch and bound |
| Data structures | Queue/stack/deque; binary heap; ordered set/map; monotone queue; monotone stack; interval set; trie; two-dimensional prefix sums; two-dimensional Fenwick tree; merge-sort tree | Mo's algorithm; Mo with updates; Mo on trees; persistent segment tree; persistent array; persistent DSU; implicit treap; order-statistic tree; wavelet matrix; segment-tree beats; dynamic Li Chao tree; rollback offline dynamic connectivity |
| Graph connectivity and traversal | DFS; bridges; articulation points; biconnected components; condensation DAG; Eulerian walk; bipartiteness; functional graph cycles | Dominator tree; directed minimum spanning arborescence; minimum cycle basis |
| Shortest paths and spanning trees | Bellman-Ford; Floyd-Warshall; negative-cycle detection; shortest-path reconstruction; Prim MST; DAG shortest paths | Johnson all-pairs; k-shortest paths; second-best MST; Manhattan MST; Steiner-tree subset DP |
| Trees | Binary lifting; subtree Euler tour; heavy-light decomposition; tree diameter; rerooting DP; subtree DSU/small-to-large | Centroid decomposition; virtual tree; link-cut tree; Euler-tour dynamic tree; top tree |
| Flow, cuts, and matching | Bipartite minimum vertex cover; assignment/Hungarian algorithm; lower-bound circulation; minimum-cut extraction | General graph blossom matching; weighted general matching; push-relabel; Gomory-Hu tree; Stoer-Wagner global cut; flow with demands; min-cost circulation |
| Strings | Manacher palindrome radii; rolling hash; minimal cyclic rotation; trie word matching; suffix-array search | Suffix automaton; palindromic tree/eertree; suffix tree; Lyndon factorization; runs enumeration |
| Dynamic programming | Unbounded knapsack; bounded knapsack; subset/bitmask DP; digit DP; tree DP; interval DP; reconstruction; edit distance; longest common subsequence | Divide-and-conquer DP optimization; Knuth optimization; convex hull trick; Li Chao optimization; monotone minima/SMAWK; SOS DP; subset convolution; profile DP; alien trick/parametric DP |
| Number theory | Euclidean gcd; extended gcd; linear Diophantine equations; modular exponentiation; modular inverse; linear sieve; Euler phi; divisor enumeration; divisor sums | Deterministic 64-bit Miller-Rabin; Pollard rho factorization; discrete logarithm; modular square root; primitive root; Mobius inversion; segmented sieve; summatory sieve; prime counting; modular tetration; Stern-Brocot/Farey methods |
| Combinatorics | Inclusion-exclusion; derangements; Catalan numbers; multinomial coefficients; Pascal identities; permutation ranking/unranking | Lucas theorem; binomial modulo composite; Stirling numbers; Bell numbers; partition numbers; Burnside lemma; Polya enumeration; matrix-tree theorem |
| Linear algebra | Matrix multiplication; matrix exponentiation; determinant; Gaussian elimination modulo a prime; GF(2) elimination; XOR linear basis | Matrix inverse; characteristic polynomial; sparse linear algebra; Wiedemann method |
| Transforms, polynomials, FPS | Floating FFT; XOR FWHT; subset zeta transform; subset Mobius transform; polynomial interpolation | Formal-power-series inverse; FPS logarithm; FPS exponential; FPS square root; FPS power; polynomial division; multipoint evaluation; product tree; Taylor shift; Berlekamp-Massey; linear recurrence/Kitamasa; Bostan-Mori |
| Computational geometry | Orientation/cross products; dot products; segment intersection; point on segment; line intersection; point in polygon; rotating calipers; closest pair; circle-line intersection; circle-circle intersection | Half-plane intersection; minimum enclosing circle; Minkowski sum; polygon union area; Delaunay triangulation; three-dimensional hull; spherical geometry |
| Game theory | Subtraction games; mex; Sprague-Grundy decomposition; misere Nim | Nimber multiplication; loopy-game outcome classification |
| Numerical methods | Bisection root finding; numerical stability/epsilon comparisons; golden-section search | Adaptive integration; polynomial root isolation; simplex linear programming |
| Randomized algorithms | Seeded PRNG discipline; randomized hashing; collision probability/birthday bounds; shuffle invariants | Randomized incremental geometry; simulated annealing; randomized polynomial identity testing |
| Problem-solving references | Greedy exchange arguments; invariants; amortized analysis; coordinate transforms; prefix/difference transformations; offline versus online processing; reductions; binary search on answer; constructive proofs; brute-force stress methodology | Matroid intersection; meet-in-the-middle subset optimization; divide-and-conquer on time; multidimensional dominance/CDQ |

This is a finite inventory, not an unlimited "etc." promise. Missing advanced
references such as a suitable top tree must be visibly recorded rather than
implemented speculatively. References may be educational rather than reusable
code, and must be labeled accordingly.

## Concrete Reference Starting Points Already Located

The following paths were observed in the approved repositories. They still
require checkout/path/license/dependency verification before runtime catalog
credit and have no local test evidence:

| Topic | Source ID and relative path | Planned status |
|-------|-----------------------------|----------------|
| Persistent segment tree | nyaan `segment-tree/persistent-segment-tree.hpp` | reference-only |
| Wavelet matrix | nyaan `data-structure-2d/wavelet-matrix.hpp` | reference-only |
| HLD | kactl `content/graph/HLD.h` | reference-only; allocation/dependency review |
| Centroid decomposition | nyaan `tree/centroid-decomposition.hpp`; cp-algorithms `src/graph/centroid_decomposition.md` | code/article references |
| Link-cut tree | nyaan `lct/link-cut-tree.hpp`; luzhiled `structure/dynamic-tree/link-cut-tree.hpp` | reference-only |
| Implicit treap | suisen `library/datastructure/bbst/implicit_treap.hpp` | reference-only; verify actual dependency closure |
| Manacher | nyaan `string/manacher.hpp`; cp-algorithms `src/string/manacher.md` | code/article references |
| Suffix automaton | nyaan `string/suffix-automaton.hpp`; cp-algorithms `src/string/suffix-automaton.md` | code/article references |
| Hungarian assignment | luzhiled `graph/flow/hungarian.hpp`; cp-algorithms `src/graph/hungarian-algorithm.md` | code/article references |
| Divide-and-conquer DP | luzhiled `dp/divide-and-conquer-optimization.hpp`; cp-algorithms `src/dynamic_programming/divide-and-conquer-dp.md` | code/article references |
| Knuth optimization | cp-algorithms `src/dynamic_programming/knuth-optimization.md` | article reference |
| Formal power series | nyaan `fps/formal-power-series.hpp`; suisen `docs/polynomial/formal_power_series.md` | reference-only, split individual operations in catalog |
| Berlekamp-Massey | nyaan `fps/berlekamp-massey.hpp` | reference-only |
| Kitamasa recurrence | nyaan `fps/kitamasa.hpp` | reference-only |
| Half-plane intersection | cp-algorithms `src/geometry/halfplane-intersection.md` | article reference |
| Sprague-Grundy | cp-algorithms `src/game_theory/sprague-grundy-nim.md` | article reference |
| Simulated annealing | nyaan `marathon/simulated-annealing.hpp`; cp-algorithms `src/num_methods/simulated_annealing.md` | heuristic references, not guaranteed optimization |

## Required Authored Reference Notes

`docs/contest-workflow.md` must cover environment checks, overflow/sentinel
discipline, indexing, recursion/stack risks, input/output, stress testing,
complexity estimation, common STL pitfalls, and a pre-submission checklist.
`docs/coverage.md` must explain every status and summarize counts by individual
entry, source, and topic without category-wide certification.
