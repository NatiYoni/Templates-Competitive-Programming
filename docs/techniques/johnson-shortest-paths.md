# Johnson all-pairs shortest paths

Original educational notes by Toolkit contributors. This is an algorithmic
reference, **not a new tested Johnson implementation**. The individual
Bellman-Ford and Dijkstra integrations do not by themselves certify this
composition. No repository-wide public license is selected.

## Reweighting and recovery

Johnson's algorithm handles a directed sparse graph with signed edge weights
when the graph has no negative cycle. Add a virtual source with a zero-cost
edge to every vertex and run Bellman-Ford. Equivalently, initialize every
potential to zero and relax all original edges with the same global
negative-cycle detection semantics. A negative cycle in any component rejects
the finite all-pairs problem, even if it was unreachable from a particular
original vertex.

Let `h[v]` be the resulting shortest-path potential. Replace each edge cost
`w(u,v)` by `w(u,v)+h[u]-h[v]`. The Bellman-Ford inequalities make these reduced
costs nonnegative. Along a path, the potential terms telescope to a quantity
depending only on its endpoints, so the ordering of path costs is preserved.
Run Dijkstra from each original vertex using the reduced costs. Recover a
finite original distance by subtracting `h[source]` and adding `h[target]`.
Preserve unreachable status instead of applying this arithmetic to infinity.

With binary-heap Dijkstra, a conventional bound is `O(n*m + n*(n+m)*log(n+1))`,
with an `O(n^2)` output matrix if all results are stored. Streaming one row at
a time can avoid that output storage, but not the repeated searches.

Reweighting can increase an individual edge's magnitude. Prove that potentials,
reduced costs, Dijkstra additions and recovery fit the chosen type. The existing
Dijkstra helper has a conservative edge-cost bound; a reweighted graph is not
automatically inside that bound merely because its input weights were legal
for Bellman-Ford. Do not clamp a negative reduced cost to zero to hide overflow
or an incorrect potential calculation. Test any future implementation against
all-pairs Bellman-Ford or Floyd-Warshall on small disconnected signed graphs.
