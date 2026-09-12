#pragma once
#include "signed_paths.hpp"

namespace toolkit {
// Independent original. Synchronous rounds bound each walk by its edge count.
// Directed multigraph, |cost| <= (LLONG_MAX-1)/n; see signed_paths.hpp.
// O(n(n+m)) time, O(n+m) space; no recursion or input mutation.
inline vector<SignedDistance> bellman_ford(int n, const vector<SignedEdge>& edges,
                                          int source) {
    validate_signed_graph(n, edges);
    validate_path_source(n, source);
    vector<long long> d(n, LLONG_MAX);
    vector<vector<int>> g(n);
    for (auto e : edges) g[e.from].push_back(e.to);
    d[source] = 0;
    vector<bool> bad(n);
    for (int round = 0; round < n; ++round) {
        auto next = d;
        bool changed = false;
        for (auto e : edges) if (d[e.from] != LLONG_MAX) {
            long long candidate = d[e.from] + e.cost;
            if (candidate < next[e.to]) {
                next[e.to] = candidate;
                changed = true;
                if (round == n - 1) bad[e.to] = true;
            }
        }
        d.swap(next);
        if (!changed) break;
    }
    queue<int> q;
    for (int v = 0; v < n; ++v) if (bad[v]) q.push(v);
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (int u : g[v]) if (!bad[u]) { bad[u] = true; q.push(u); }
    }
    vector<SignedDistance> result(n);
    for (int v = 0; v < n; ++v) {
        if (bad[v]) result[v].state = DistanceState::negative_infinity;
        else if (d[v] != LLONG_MAX) result[v] = {DistanceState::finite, d[v]};
    }
    return result;
}
}
