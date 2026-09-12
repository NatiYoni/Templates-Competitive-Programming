#pragma once
#include "checked_tree.hpp"
#include <tree/centroid-decomposition.hpp>

namespace toolkit {
// Original nearest-marked adapter; centroid hierarchy is unmodified Nyaan CC0.
// Nonempty unweighted tree; activate is idempotent, reset removes all marks.
// Build O(n log n) time/space; activate/nearest O(log n), reset O(n).
class CentroidNearest {
    CheckedTree tree;
    vector<int> parents, best;
    vector<vector<pair<int, int>>> chains;
public:
    CentroidNearest(int n, const vector<pair<int, int>>& edges)
        : tree(n, edges), parents(n, -1), best(n, INT_MAX), chains(n) {
        CentroidDecomposition<vector<vector<int>>> cd(tree.adjacency);
        vector<bool> removed(n);
        vector<int> todo{cd.root};
        while (!todo.empty()) {
            int c = todo.back(); todo.pop_back();
            vector<array<int, 3>> walk{{c, -1, 0}};
            while (!walk.empty()) {
                auto [v, p, d] = walk.back(); walk.pop_back();
                chains[v].push_back({c, d});
                for (int u : tree.adjacency[v]) if (u != p && !removed[u])
                    walk.push_back({u, v, d + 1});
            }
            removed[c] = true;
            for (int child : cd.tree[c]) { parents[child] = c; todo.push_back(child); }
        }
    }
    const vector<int>& parent() const { return parents; }
    const vector<vector<pair<int, int>>>& ancestors() const { return chains; }
    void activate(int v) {
        tree.check_vertex(v);
        for (auto [c, d] : chains[v]) best[c] = min(best[c], d);
    }
    optional<int> nearest(int v) const {
        tree.check_vertex(v);
        long long answer = LLONG_MAX;
        for (auto [c, d] : chains[v]) if (best[c] != INT_MAX)
            answer = min(answer, (long long)d + best[c]);
        if (answer == LLONG_MAX) return nullopt;
        return int(answer);
    }
    void reset() { fill(best.begin(), best.end(), INT_MAX); }
};
}
