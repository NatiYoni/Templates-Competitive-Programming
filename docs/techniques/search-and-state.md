# Search boundaries and state design

Original educational notes by Toolkit contributors. These worked reasoning
patterns remain reference-only; they are not generic tested solvers. No
repository-wide public license is selected for this original material.

## Two pointers

The linear-time argument needs two monotone boundaries, not just two variables.
For nonnegative array values, consider counting nonempty subarrays with sum at
most a nonnegative threshold. Extend the right boundary, then advance the left
boundary until the window is feasible. Every suffix starting between that left
boundary and the right endpoint is also feasible, so this endpoint contributes
the window length. Each boundary advances at most `n` times.

The invariant is that the maintained sum equals the current half-open window,
and after shrinking the left boundary is the earliest feasible start. Negative
values break the monotonicity: in `[2,-2]` at threshold0, removing the first
element while processing the first endpoint loses the later valid full window.
Use a prefix-sum/order-statistic method instead when signs are unrestricted.
Zeros are allowed but affect counts; products and pair sums need their own
monotonicity proof. Sorting may enable a two-pointer method while destroying
the original subarray-contiguity requirement, so it is not a universal repair.

## Sweep line events

A sweep orders changes along one coordinate and maintains the active state in
the remaining dimension. To measure the union length of one-dimensional
half-open intervals, sort endpoints, accumulate the distance from the previous
coordinate whenever the active count is positive, then apply all deltas at the
current coordinate. Grouping equal coordinates avoids artificial zero-length
steps and gives deterministic handling of touching intervals.

Counting intervals containing a query point needs a different tie policy:
for half-open intervals at coordinate `x`, apply removals and additions before
answering membership queries; closed intervals keep their right endpoints
active for those queries. Specify this before writing an event comparator.

In a two-dimensional sweep, the status structure and ordering invariant are
the real algorithm. A tree comparator depending on a changing sweep coordinate
can invalidate the tree order. Do not claim a general segment-intersection
sweep merely by sorting events. Budget event-coordinate differences, total
length, and overlap counts separately; each may need a wider type.

## Meet in the middle

When independent choices split into two halves, enumerate half-solutions and
combine them instead of enumerating full solutions. For `n` subset choices this
changes the stored-list scale from `2^n` to roughly `2^(n/2)`, at the price of
sorting/searching or hashing the combinations. It is still exponential.

For a maximum subset sum not exceeding a target, enumerate both half-sum lists.
For each left sum, binary-search the greatest right sum at most the remaining
budget. Include the empty subset in both lists. With signed values, the empty
subset is not necessarily feasible for a negative target, and pruning a
partial sum merely because it exceeds the target is unsound.

For counting solutions, duplicate half-sums represent distinct subsets and
must retain multiplicities; deduplication is appropriate for feasibility or
maximum-value queries but not automatically for counts. Reconstruction needs
the chosen half masks as well as their sums. Bound both the exponential memory
and the arithmetic before allocation. The corresponding scoped integration
is [subset optimization](../../examples/meet-in-the-middle-subset-optimization.cpp).

## Recursion

A recursive contract should describe one call's input, returned result, and
which state it owns. Prove termination using a strictly decreasing measure:
an interval length, number of unassigned choices, or remaining depth. Reaching
an already active state in a graph is not the same as reaching a base case.

A tree DFS that skips only the parent assumes a valid tree. On a cyclic graph,
use visitation state and define whether entry, exit, or both events matter.
On a path-shaped tree the call depth is `n`, even though the total work is
linear. GCC does not promise tail-call elimination for arbitrary contest code;
use explicit frames if that depth is unsafe.

Pass immutable large inputs by reference, but keep per-call accumulators local.
When mutable global state is intentionally shared, identify which changes must
be undone before returning. Test empty/base inputs, deepest legal chains, and
consecutive invocations; retained state between calls is a common error that a
single sample does not reveal.

## Backtracking

Backtracking explores a decision tree while restoring the exact state at each
return. For generating distinct permutations of a multiset, keep a frequency
table, choose one currently available symbol, decrement it, recurse, and restore
it. Choosing array positions instead would generate duplicates when values
repeat. The invariant is that the remaining frequencies plus the chosen prefix
equal the original multiset.

Pruning requires a necessary condition for completion or a valid bound on the
best reachable objective. A bound must be optimistic for maximization; pruning
from an achievable lower bound can discard the optimum. Also state whether
ties may be pruned when all optimal solutions must be listed.

Restore state even on success if the caller continues searching. Returning
early with a borrowed mutable buffer can leak changes into siblings. Count
visited states and output size rather than calling the method polynomial
because pruning helps on examples. Tiny exhaustive enumeration is an oracle
for an optimized solver, not evidence of scalability by itself.

## Subset bitmask DP

A subset state must retain all information needed by future transitions. In a
Hamiltonian-path example, `dp[mask][last]` counts paths using exactly `mask` and
ending at `last`; `dp[mask]` alone loses the endpoint needed to decide the next
edge. Initialize the intended starting vertices, then extend only to an unused
adjacent vertex. Process masks so predecessors are available.

For dense graphs, this particular formulation takes `O(n^2 * 2^n)` time and
`O(n * 2^n)` memory, before accounting for arithmetic width. Counting may require
a specified modulus; storing exact counts in a signed integer can overflow
long before memory is exhausted. An optimization version instead needs an
explicit unreachable state and safe addition.

Unsigned masks do not make `1 << n` safe: choose an appropriate unsigned type,
check the shift width, and validate the allocation product separately. Submask
enumeration has a different aggregate complexity (`3^n` across all masks).
This is a worked state-design reference, not a universal subset-DP template.
