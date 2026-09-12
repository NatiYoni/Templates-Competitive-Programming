#pragma once
#include "checked_tree.hpp"
#include <graph/tree/heavy-light-decomposition.hpp>

namespace toolkit {
// Original adapter; layout/LCA from unmodified Luzhiled (Unlicense).
// Segments concatenate in u->v order. Each [first,last) is read backwards iff
// reversed. edge=true stores each edge at its deeper endpoint and omits the LCA.
struct HeavySegment { int first, last; bool reversed; };
class HeavyLight {
    CheckedTree tree;
    ::HeavyLightDecomposition<int> hld;
public:
    HeavyLight(int n, const vector<pair<int, int>>& edges, int root = 0)
        : tree(n, edges, root), hld(n) {
        for (auto [u, v] : edges) hld.add_edge(u, v);
        hld.build(root);
    }
    const vector<int>& position() const { return hld.in; }
    const vector<int>& vertex_at() const { return hld.rev; }
    const vector<int>& parent() const { return hld.par; }
    int lca(int u, int v) const {
        tree.check_vertex(u); tree.check_vertex(v);
        return hld.lca(u, v);
    }
    pair<int, int> subtree(int v, bool edge = false) const {
        tree.check_vertex(v);
        return {hld.in[v] + int(edge), hld.out[v]};
    }
    vector<HeavySegment> path(int u, int v, bool edge = false) const {
        tree.check_vertex(u); tree.check_vertex(v);
        vector<HeavySegment> left, right;
        while (hld.head[u] != hld.head[v]) {
            if (hld.dep[hld.head[u]] >= hld.dep[hld.head[v]]) {
                left.push_back({hld.in[hld.head[u]], hld.in[u] + 1, true});
                u = hld.par[hld.head[u]];
            } else {
                right.push_back({hld.in[hld.head[v]], hld.in[v] + 1, false});
                v = hld.par[hld.head[v]];
            }
        }
        int lo = min(hld.in[u], hld.in[v]) + int(edge);
        int hi = max(hld.in[u], hld.in[v]) + 1;
        if (lo < hi) left.push_back({lo, hi, hld.in[u] > hld.in[v]});
        left.insert(left.end(), right.rbegin(), right.rend());
        return left;
    }
};
}
