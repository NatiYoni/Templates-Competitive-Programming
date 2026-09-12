# Portability and contracts

## What the baseline means

The user explicitly elected a **C++17-compatible ETCPC core**. A current
ETCPC compiler version/standard was not publicly verified from the ICPC event
listing or EtCPC announcements. Do not substitute World Finals settings for
regional confirmation. Verify the actual contest compiler, memory/stack
limits, permitted references, and language flags before the event.

The baseline is Linux **GNU GCC/libstdc++**, compiling with
`-std=c++17 -O2 -Wall -Wextra`. `include/toolkit/base.hpp` uses
`bits/stdc++.h` and a global `using namespace std`. KACTL helpers use the
documented prelude aliases/macros; some numeric code uses GNU `__int128`.
Consequently, “C++17-compatible” does not promise an ISO-only compiler,
MSVC, libc++, or identical behavior on every architecture.

CP-Algorithms auxiliary and ecnerwala's pinned current libraries require
**C++23** and are reference-only by default. Nyaan's README warns about
AVX2-only files; individual GNU/PBDS/builtin/SIMD requirements remain file
specific. No default runner command enables native CPU tuning, AVX2, or
fast-math. Optional sanitizer results are a separate profile, not inferred
from a baseline pass.

## Independent programs, not one mega-header

Each selected example is independently compiled with its declared source roots.
Upstream APIs remain visible. KACTL global names/macros, ACL namespaces, and
other libraries are not guaranteed to coexist in a single translation unit.
The notebook includes complete snippets, examples, and dependencies, but
does not flatten all files into one compilation unit.
For example, KACTL point-in-polygon and closest-pair headers declare conflicting
global `P` aliases and non-inline definitions. Their standalone integrations
are not a promise that those headers can be pasted together unchanged.

ACL includes extensionless wrappers such as `atcoder/internal_bit` as well as
`.hpp` files. Keep the whole resolved dependency closure. KACTL LCA includes
its relative RMQ header; geometry includes `Point.h`. The original prelude is
not copied from KACTL's ambiguously licensed contest template.

## Limits to check before adapting

- **Indexing:** most local examples use zero-based vertices and half-open
  ranges. Never infer a closed interval or sentinel convention from the
  algorithm's name; read its full catalog contract.
- **Arithmetic:** intermediate products, cumulative sums, path costs,
  transform lengths, and sentinels must fit their documented types. A
  `long long` result does not make an `int * int` intermediate safe.
- **Graphs:** Dijkstra requires nonnegative weights; 0–1 BFS requires exactly
  zero/one costs. A minimum spanning forest permits disconnected input.
  LCA requires a valid nonempty tree rooted at zero.
- **Residual networks:** flows mutate residual capacities. Respect max-flow
  limit/min-cut semantics and ACL min-cost-flow's nonnegative original-cost
  and supported call/domain restrictions.
- **Stack:** KACTL LCA's traversal and several graph/string/number-theory
  routines are recursive. Small local tests do not prove that a worst-case
  contest chain fits the judge's stack.
- **Algebra:** modint inversion requires an invertible residue; dynamic
  modulus changes invalidate the interpretation of existing values.
  Factorial tables must stay below their prime modulus.
- **Transforms:** NTT primes and maximum lengths are constrained; integer
  convolution additionally bounds every true coefficient. Do not reinterpret
  convolution rounding/overflow contracts.
- **Strings:** KACTL Aho-Corasick uses uppercase `A`–`Z` and nonempty patterns.
  Check duplicate-pattern and match-index semantics. Empty input behavior
  differs between suffix-array and LCP APIs.
- **Sieve/global state:** KACTL's sieve has a fixed bitset capacity and global
  state. Repeat-call behavior and the exclusive limit are documented and
  locally tested only within the selected contract.
- **Geometry:** cross products and area accumulation have finite integer
  bounds. Duplicates, collinearity, hull orientation, and nonempty polygon
  preconditions matter.
- **Numerics:** Gaussian elimination is tolerance/conditioning-dependent;
  Simpson integration approximates a finite continuous function with an
  explicit subdivision policy. Passing analytic cases is not a universal
  error bound.

Exact per-entry preconditions, invalid-input policy, numeric limits, mutation,
global state, recursion and complexity live in `catalog/algorithms.json` and
appear in generated documents. Upstream raw headers retain their own
preconditions; example boundary checks are not universal defensive adapters.

## Expansion-specific boundaries

Bellman-Ford and Floyd-Warshall distinguish unreachable, finite, and
negative-cycle-affected distances. DAG paths reject cycles even in unreachable
components. Lowlink supports edge identities and parallel edges, treating each
self-loop as a separate block; its DFS, HLD and centroid construction can
require linear call-stack depth. Rerooting preserves adjacency order for an
associative merge, including noncommutative operations. Circulation returns
bounded integral edge flows; Hungarian rejects an optimum outside signed64.

Persistent range sums and Li Chao evaluations use signed128-bit arithmetic
and explicit node budgets. Li Chao supports a full signed64 integer domain.
Digit DP is the no-equal-neighbor/digit-sum-residue problem over unsigned64
bounds, not an arbitrary digit recurrence. Adjacent-merge interval DP allows
signed weights up to500 entries; Knuth requires nonnegative weights and
allows up to3000. Partition D&C requires monotone leftmost optima. Subset
transforms check each signed64 intermediate; MITM supports at most40 values.

Miller-Rabin is deterministic for unsigned64, but bounded Pollard rho may
return failure rather than partial factors. Its seed and retry budget are
explicit. Non-coprime discrete logarithms support moduli only through
`UINT32_MAX`, not arbitrary unsigned64. Modular matrix products use
unsigned128-bit intermediates. Subtraction games are finite normal play,
without misere rules or unproved periodic extrapolation.

Manacher and suffix-automaton integrations support arbitrary bytes. The
automaton owns the state borrowed by its occurrence counter. Rolling hashes
need a shared context for cross-string comparisons and can collide; a
61-bit field is not a collision-free or cryptographic guarantee. LCS has
quadratic table memory. Segment intersection and point-in-polygon use exact
integer predicates for coordinates bounded by `10^18`; closest pair has the
stricter `10^8` coordinate limit and returns no pair for fewer than two points.
