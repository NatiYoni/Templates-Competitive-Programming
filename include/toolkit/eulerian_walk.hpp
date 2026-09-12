#pragma once
#include "base.hpp"
#include <graph/others/eulerian-trail.hpp>

namespace toolkit {
// Original checked adapter around Luzhiled's unmodified Unlicense trail header.
// directed is a compile-time choice. No input mutation; edge IDs are input order.
struct EulerWalk { vector<int> vertices, edges; };
template<bool directed>
optional<EulerWalk> eulerian_walk(int n, const vector<pair<int, int>>& edges) {
    if (n < 0 || edges.size() > size_t(INT_MAX / 2))
        throw invalid_argument("Euler graph size");
    EulerianTrail<directed> graph(n);
    for (auto [u, v] : edges) {
        if (u < 0 || u >= n || v < 0 || v >= n) throw out_of_range("Euler endpoint");
        graph.add_edge(u, v);
    }
    if (edges.empty()) return EulerWalk{n ? vector<int>{0} : vector<int>{}, {}};
    auto trails = graph.enumerate_semi_eulerian_trail();
    if (trails.size() != 1 || trails[0].size() != edges.size()) return nullopt;
    auto ids = move(trails[0]);
    int start = edges[ids[0]].first;
    if constexpr (!directed) {
        int odd = -1;
        for (int v = 0; v < n; ++v) if (graph.deg[v] & 1) { odd = v; break; }
        if (odd != -1) start = odd;
        else {
            start = n;
            for (auto [u, v] : edges) start = min(start, min(u, v));
        }
    }
    EulerWalk result{{start}, move(ids)};
    for (int id : result.edges) {
        auto [u, v] = edges[id];
        int last = result.vertices.back();
        if constexpr (directed) {
            if (last != u) throw logic_error("upstream Euler orientation");
            result.vertices.push_back(v);
        } else {
            if (last != u && last != v) throw logic_error("upstream Euler adjacency");
            result.vertices.push_back(last == u ? v : u);
        }
    }
    return result;
}
}
