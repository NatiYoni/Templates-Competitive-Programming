#pragma once
#include "signed_paths.hpp"

namespace toolkit {
// Independent original. Rejects any directed cycle, even outside source reach.
// O(n+m) time/space; no recursion. nullopt is a cycle, not unreachability.
inline optional<vector<SignedDistance>> dag_shortest_paths(
        int n, const vector<SignedEdge>& edges, int source) {
    validate_signed_graph(n, edges);
    validate_path_source(n, source);
    vector<vector<pair<int, long long>>> g(n);
    vector<size_t> indegree(n);
    for (auto e : edges) { g[e.from].push_back({e.to, e.cost}); ++indegree[e.to]; }
    queue<int> q;
    for (int v = 0; v < n; ++v) if (!indegree[v]) q.push(v);
    vector<SignedDistance> result(n);
    result[source] = {DistanceState::finite, 0};
    int visited = 0;
    while (!q.empty()) {
        int v = q.front(); q.pop(); ++visited;
        for (auto [u, cost] : g[v]) {
            if (result[v].value) {
                long long candidate = *result[v].value + cost;
                if (!result[u].value || candidate < *result[u].value)
                    result[u] = {DistanceState::finite, candidate};
            }
            if (!--indegree[u]) q.push(u);
        }
    }
    if (visited != n) return nullopt;
    return result;
}
}
