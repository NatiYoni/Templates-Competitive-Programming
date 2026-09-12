#pragma once
#include "base.hpp"

namespace toolkit {
// Directed unweighted graph, vertices [0,n), 1<=n<=INT_MAX; -1 means unreachable.
// All endpoints/source checked, even unreachable edges. O(n+m) time, O(n) space;
// graph is unchanged; iterative with no global state.
inline vector<int> bfs(const vector<vector<int>>& graph, int source) {
    if (graph.size() > size_t(INT_MAX)) throw invalid_argument("too many vertices");
    int n = int(graph.size());
    if (source < 0 || source >= n) throw out_of_range("BFS source");
    for (const auto& edges : graph)
        for (int v : edges)
            if (v < 0 || v >= n) throw out_of_range("BFS endpoint");
    vector<int> distance(n, -1);
    queue<int> q;
    distance[source] = 0;
    q.push(source);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : graph[u]) if (distance[v] == -1) {
            distance[v] = distance[u] + 1;
            q.push(v);
        }
    }
    return distance;
}
}
