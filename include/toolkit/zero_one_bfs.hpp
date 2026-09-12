#pragma once
#include "base.hpp"

namespace toolkit {
// Directed edges (to,cost), cost exactly 0 or 1, vertices [0,n), 1<=n<=INT_MAX.
// LLONG_MAX means unreachable. Invalid costs/endpoints/source throw.
// O(n+m) time and O(n+m) auxiliary space including stale deque entries.
// Input unchanged; iterative, no global state.
inline vector<long long> zero_one_bfs(const vector<vector<pair<int, int>>>& graph,
                                      int source) {
    if (graph.size() > size_t(INT_MAX)) throw invalid_argument("too many vertices");
    int n = int(graph.size());
    if (source < 0 || source >= n) throw out_of_range("0-1 BFS source");
    for (const auto& edges : graph)
        for (auto [v, cost] : edges) {
            if (v < 0 || v >= n) throw out_of_range("0-1 BFS endpoint");
            if (cost != 0 && cost != 1) throw invalid_argument("0-1 BFS cost");
        }
    vector<long long> distance(n, LLONG_MAX);
    deque<pair<long long, int>> q;
    distance[source] = 0;
    q.push_front({0, source});
    while (!q.empty()) {
        auto [d, u] = q.front(); q.pop_front();
        if (d != distance[u]) continue;
        for (auto [v, cost] : graph[u]) if (d + cost < distance[v]) {
            distance[v] = d + cost;
            if (cost == 0) q.push_front({distance[v], v});
            else q.push_back({distance[v], v});
        }
    }
    return distance;
}
}
