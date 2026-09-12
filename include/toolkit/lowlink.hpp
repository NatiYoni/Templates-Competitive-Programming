#pragma once
#include "base.hpp"

namespace toolkit {
// Independent original edge-ID lowlink. Each self-loop is its own edge block;
// isolated vertices have no block. Bridges are singleton non-loop blocks.
struct LowlinkResult {
    vector<int> bridges, articulation;
    vector<vector<int>> edge_blocks;
};
// Undirected multigraph; DFS output order unspecified. O(n+m) time/space.
// Recursive depth O(n): provision stack space for deep graphs.
inline LowlinkResult lowlink(int n, const vector<pair<int, int>>& edges) {
    if (n < 0 || edges.size() > size_t(INT_MAX)) throw invalid_argument("lowlink size");
    vector<vector<pair<int, int>>> g(n);
    LowlinkResult result;
    for (int id = 0; id < int(edges.size()); ++id) {
        auto [u, v] = edges[id];
        if (u < 0 || u >= n || v < 0 || v >= n) throw out_of_range("lowlink endpoint");
        if (u == v) result.edge_blocks.push_back({id});
        else { g[u].push_back({v, id}); g[v].push_back({u, id}); }
    }
    vector<int> tin(n, -1), low(n), pending;
    vector<bool> cut(n);
    int timer = 0;
    auto visit = [&](auto&& self, int v, int parent_edge) -> void {
        tin[v] = low[v] = timer++;
        int children = 0;
        for (auto [u, id] : g[v]) {
            if (id == parent_edge) continue;
            if (tin[u] == -1) {
                ++children;
                pending.push_back(id);
                self(self, u, id);
                low[v] = min(low[v], low[u]);
                if (low[u] > tin[v]) result.bridges.push_back(id);
                if (low[u] >= tin[v]) {
                    if (parent_edge != -1) cut[v] = true;
                    vector<int> block;
                    do { block.push_back(pending.back()); pending.pop_back(); }
                    while (block.back() != id);
                    result.edge_blocks.push_back(move(block));
                }
            } else if (tin[u] < tin[v]) {
                pending.push_back(id);
                low[v] = min(low[v], tin[u]);
            }
        }
        if (parent_edge == -1 && children > 1) cut[v] = true;
    };
    for (int v = 0; v < n; ++v) if (tin[v] == -1) visit(visit, v, -1);
    for (int v = 0; v < n; ++v) if (cut[v]) result.articulation.push_back(v);
    return result;
}
}
