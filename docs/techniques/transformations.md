# Coordinate and difference transformations

Original educational notes by Toolkit contributors. These worked techniques
remain reference-only and do not add generic tested solvers. No public
repository-wide license is selected.

## Coordinate transforms

For planar Manhattan distance, transform a point `(x,y)` to `(u,v)=(x+y,x-y)`.
The original distance between two points equals the larger of the absolute
differences in their transformed coordinates. One way to see this is to split
on the signs of the original coordinate differences: either their sum or
their difference adds the two magnitudes.

This can turn a Manhattan-radius constraint into an axis-aligned box, making
sweeps and range-query structures applicable. It does not by itself solve the
resulting dynamic/query problem. The inverse is `x=(u+v)/2,y=(u-v)/2`; integer
points therefore occupy only transformed pairs of matching parity. Ignoring
that parity invents nonexistent lattice points when counting or reconstructing.

The transformation doubles signed area, so an area result needs an explicit
scale conversion. Coordinate sums and differences can overflow even when
each input coordinate fits its type; promote before arithmetic, not after.
For non-integer geometry, consider conditioning and rounding near boundaries.
Verify a reduction's metric, lattice, orientation and volume effects separately
rather than treating a convenient picture as a proof.

## Difference transformations

For a one-dimensional array, store its first value and successive differences.
Adding `delta` to a half-open range `[l,r)` changes only the difference at `l`
by `+delta` and, if it exists, the difference at `r` by `-delta`. A prefix scan
then recovers all final values. A sentinel at index `n` simplifies the update
rule while the final scan still emits only `n` array elements.

This gives `O(n+q)` total work for batched range additions followed by final
materialization. It does not answer arbitrary interleaved range-sum queries in
constant time; a Fenwick or segment tree needs an additional derivation.
The sign cancellation is also why the convention must stay half-open. Empty
ranges make two opposite updates at the same position and should change
nothing.

In two dimensions, a rectangle update changes four difference corners with
alternating signs, followed by prefix scans in both axes. Allocate the needed
sentinel row/column and keep axis conventions explicit. Range assignment is
not additive and cannot use the same cancellation rule. Bound endpoint deltas
and reconstructed sums, including overlapping updates and negative values;
unsigned wraparound is not a substitute for an exact signed-sum contract.
