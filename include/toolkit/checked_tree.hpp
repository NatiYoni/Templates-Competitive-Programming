#pragma once
#include "base.hpp"

namespace toolkit {
// Independent original tree validation shared by the new tree integrations.
// Nonempty undirected edge list: exactly n-1 edges, no loops/multiedges/cycles.
struct CheckedTree {
    vector<vector<int>> adjacency;
    vector<int> parent, depth, order;
    CheckedTree(int n, const vector<pair<int, int>>& edges, int root = 0) {
        if (n <= 0) throw invalid_argument("tree must be nonempty");
        if (root < 0 || root >= n) throw out_of_range("tree root");
        if (edges.size() != size_t(n - 1)) throw invalid_argument("tree edge count");
        adjacency.resize(n);
        for (auto [u, v] : edges) {
            if (u < 0 || u >= n || v < 0 || v >= n) throw out_of_range("tree endpoint");
            if (u == v) throw invalid_argument("tree self-loop");
            adjacency[u].push_back(v); adjacency[v].push_back(u);
        }
        parent.assign(n, -2); depth.assign(n, 0);
        parent[root] = -1; order.push_back(root);
        for (size_t i = 0; i < order.size(); ++i) {
            int v = order[i];
            for (int u : adjacency[v]) {
                if (u == parent[v]) continue;
                if (parent[u] != -2) throw invalid_argument("tree cycle or parallel edge");
                parent[u] = v; depth[u] = depth[v] + 1; order.push_back(u);
            }
        }
        if (order.size() != size_t(n)) throw invalid_argument("disconnected tree");
    }
    void check_vertex(int v) const {
        if (v < 0 || size_t(v) >= parent.size()) throw out_of_range("tree vertex");
    }
};
}
