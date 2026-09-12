# Proof patterns and contest workflow

Original educational notes by Toolkit contributors. These are explanatory
references with worked examples, not separately tested solvers or proofs for
arbitrary problems. No public repository-wide license has been selected.

## Greedy exchange arguments

To justify a greedy choice, start with an optimal solution that may not use it
and transform that solution without making the objective worse. For selecting
the maximum number of nonoverlapping intervals, let the greedy first interval
finish earliest. Replacing the first interval in an optimal schedule by this
one leaves at least as much room for every later interval. Repeat on the
remaining suffix. Define whether touching endpoints count as overlapping.

This proves the unweighted cardinality version, not weighted scheduling. A
single long interval of weight100 can dominate two short intervals of weight1
each even though the short intervals yield the larger count. Likewise,
earliest-start and shortest-duration choices need separate proofs and can fail.

An exchange argument must preserve feasibility, compare the correct objective,
and make progress toward the greedy form. Merely observing that the greedy
choice appears in one optimum does not show that every later choice can be
made consistently. Write down the changed part of the solution explicitly.

## Invariants

An invariant is a precise assertion at a particular program point. Establish
it initially, show every transition preserves it, and use it with the stopping
condition to derive the answer. For boundary binary search, one possible
contract keeps a known-false endpoint and a known-true endpoint, with the
unknown boundary between them. The endpoints and midpoint update must match
that convention; mixing inclusive and half-open variants invalidates the proof.

State invariants separately from desired outcomes. “The answer is correct”
does not explain why a queue, prefix, or candidate set can safely discard work.
For a monotone minimum queue, the useful assertion includes increasing indices,
monotone values, expiry, and domination of removed candidates.

Use assertions for checkable local consequences in debugging, then compare
against a slow oracle. An invariant can hold while the postcondition is wrong
if the stopping rule is mistaken. Test initialization, the first iteration,
the last transition, and empty/singleton domains rather than only large random
inputs.

## Offline versus online processing

Offline processing knows all operations before answering them and may reorder
work while restoring answers to original query order. Coordinate compression
needs the relevant future keys; Mo ordering needs the full static query set;
segment-tree-over-time methods need each update's active interval. None of
these automatically supports an interactive or answer-dependent input stream.

A useful design exercise is to remove the time restriction temporarily:
sort queries by a threshold, advance a monotone data structure, then map each
answer back using a stored query ID. For equal thresholds, decide whether the
query should observe updates at that threshold before choosing event order.

Persistent versions and rollback are different tools. Persistence preserves
queryable versions; rollback undoes changes along a controlled traversal and
does not provide arbitrary historical access by itself. If future operations
depend on earlier answers, an offline rearrangement can change the problem.
Record the required information and memory cost, not just a faster complexity
derived under an unavailable “all queries known” assumption.

## Constructive proofs

A constructive solution must establish necessity of its input conditions,
sufficiency of the construction, and that the emitted object satisfies the
requested format. For a simple connected undirected graph with `n >= 1`
vertices and exactly `m` edges, necessity gives `n-1 <= m <= n*(n-1)/2`.
Start with a path on all vertices, then add previously absent unordered pairs
until the requested count is reached. The initial path guarantees connectivity;
adding distinct edges preserves it.

This example does not handle additional degree, planarity, bipartiteness or
diameter constraints. Those properties may be destroyed by adding arbitrary
edges. The `n=1,m=0` case should succeed, and invalid `m` should be rejected
before producing a partial object.

Construction time includes output size: writing `m` edges already costs
`O(m)`. Bound the product in the feasibility check before multiplication and
avoid generating duplicate or self edges. Validate the output independently
with a graph checker rather than asserting that the generation loop ran.

## Brute force stress methodology

Separate the fast implementation, a deliberately simple oracle, a generator,
and a comparison predicate. For a small matching instance, enumerating legal
assignments is more independent than running a second version of the same
augmenting-path algorithm. Compare witnesses too: optimum value alone cannot
detect an invalid reconstructed matching.

Generate valid inputs from the actual contract and systematically include
boundaries, duplicates, disconnected structures, all-equal values and extreme
legal arithmetic. Invalid inputs belong in separate rejection tests; a failure
outside the declared domain is not automatically an algorithm counterexample.
Use a logged seed and case index, and print the actual failing input because
library/version changes can alter random sequences.

When a mismatch appears, shrink the input while preserving both validity and
failure: remove an edge, shorten a sequence, or reduce a magnitude. Do not
“fix” the comparison tolerance until the expected error model is understood.
Timeouts and crashes are failures, not missing answers to ignore. Keep the
reduced case as a regression and rerun related cases. Passing finite stress
tests is local evidence, not a proof or a whole-library certification.
