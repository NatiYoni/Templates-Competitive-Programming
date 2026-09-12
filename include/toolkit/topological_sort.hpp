#pragma once
#include "base.hpp"

namespace toolkit {
// Directed graph, vertices [0,n), n<=INT_MAX. Nullopt on any cycle (no partial
// order); empty graph yields an empty order. FIFO ties: initial zero-indegree
// vertices ascend, then enqueue in adjacency order; not lexicographically minimal.
// O(n+m) time, O(n) space; input unchanged, iterative, no global state.
inline optional<vector<int>> topological_sort(const vector<vector<int>>& graph) {
    if (graph.size() > size_t(INT_MAX)) throw invalid_argument("too many vertices");
    int n = int(graph.size());
    vector<size_t> indegree(n);
    for (const auto& edges : graph)
        for (int v : edges) {
            if (v < 0 || v >= n) throw out_of_range("topological endpoint");
            if (indegree[v] == SIZE_MAX) throw overflow_error("topological indegree");
            ++indegree[v];
        }
    queue<int> q;
    for (int v = 0; v < n; ++v) if (indegree[v] == 0) q.push(v);
    vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : graph[u]) if (--indegree[v] == 0) q.push(v);
    }
    if (order.size() != graph.size()) return nullopt;
    return order;
}
}
