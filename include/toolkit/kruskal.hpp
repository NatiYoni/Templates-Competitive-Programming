#pragma once
#include "fundamentals_arithmetic.hpp"
#include <atcoder/dsu.hpp>

namespace toolkit {
struct WeightedEdge { int u, v; long long weight; };
struct MinimumSpanningForest {
    long long weight = 0;
    int components = 0;
    vector<int> edges;
};

// Undirected multigraph, n>=0, vertices [0,n), m<=INT_MAX. Negative weights,
// parallel edges, loops and disconnected/empty graphs allowed. Inputs unchanged.
// Returns input edge indices, ordered by (weight,index); each running selected
// sum must fit long long or overflow_error is thrown. Bad bounds also throw.
// O(n+m log(m+1)+m alpha(n)) time, O(n+m) space; uses pinned ACL DSU.
// No global state; ACL find and standard sort use logarithmic-depth recursion.
inline MinimumSpanningForest kruskal(int n, const vector<WeightedEdge>& edges) {
    if (n < 0 || edges.size() > size_t(INT_MAX)) throw invalid_argument("Kruskal size");
    for (auto e : edges)
        if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n)
            throw out_of_range("Kruskal endpoint");
    vector<int> order(edges.size());
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        return make_pair(edges[a].weight, a) < make_pair(edges[b].weight, b);
    });
    atcoder::dsu components(n);
    MinimumSpanningForest result;
    result.components = n;
    for (int i : order) {
        auto e = edges[i];
        if (components.same(e.u, e.v)) continue;
        result.weight = fundamentals_detail::checked_add(result.weight, e.weight);
        components.merge(e.u, e.v);
        --result.components;
        result.edges.push_back(i);
    }
    return result;
}
}
