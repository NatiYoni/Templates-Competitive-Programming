#pragma once
#include "base.hpp"

namespace toolkit {
// Directed edges (to,cost); 1<=n<=INT_MAX, vertices [0,n).
// Costs must lie in [0,(LLONG_MAX-1)/n], even on unreachable edges; checked.
// This conservative bound keeps every relaxation below the LLONG_MAX sentinel.
// O(n+m log(m+2)) time, O(n+m) space with lazy heap; no input mutation/recursion.
inline vector<long long> dijkstra(const vector<vector<pair<int, long long>>>& graph,
                                  int source) {
    if (graph.size() > size_t(INT_MAX)) throw invalid_argument("too many vertices");
    int n = int(graph.size());
    if (source < 0 || source >= n) throw out_of_range("Dijkstra source");
    long long limit = (LLONG_MAX - 1) / n;
    for (const auto& edges : graph)
        for (auto [v, cost] : edges) {
            if (v < 0 || v >= n) throw out_of_range("Dijkstra endpoint");
            if (cost < 0 || cost > limit) throw invalid_argument("Dijkstra cost bound");
        }
    vector<long long> distance(n, LLONG_MAX);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>,
                   greater<pair<long long, int>>> q;
    distance[source] = 0;
    q.push({0, source});
    while (!q.empty()) {
        auto [d, u] = q.top(); q.pop();
        if (d != distance[u]) continue;
        for (auto [v, cost] : graph[u]) if (d + cost < distance[v]) {
            distance[v] = d + cost;
            q.push({distance[v], v});
        }
    }
    return distance;
}
}
