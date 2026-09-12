#pragma once
#include "base.hpp"
#include <atcoder/maxflow.hpp>

namespace toolkit {
// Original balance reduction/recovery using unmodified ACL maxflow (CC0).
struct BoundedEdge { int from, to; long long lower, upper; };
// Directed integral circulation, loops/parallel arcs allowed. nullopt iff
// infeasible. Bounds 0<=lower<=upper<=LLONG_MAX; sum(lower)<=LLONG_MAX.
inline optional<vector<long long>> lower_bound_circulation(
        int n, const vector<BoundedEdge>& edges) {
    if (n < 0 || n > INT_MAX - 2 ||
        edges.size() + size_t(n) > size_t(INT_MAX / 2))
        throw invalid_argument("circulation size");
    __int128 total_lower = 0;
    for (auto e : edges) {
        if (e.from < 0 || e.from >= n || e.to < 0 || e.to >= n)
            throw out_of_range("circulation endpoint");
        if (e.lower < 0 || e.lower > e.upper) throw invalid_argument("circulation bounds");
        total_lower += e.lower;
    }
    if (total_lower > LLONG_MAX) throw invalid_argument("circulation sum of lower bounds");
    atcoder::mf_graph<long long> graph(n + 2);
    vector<long long> balance(n);
    for (auto e : edges) {
        graph.add_edge(e.from, e.to, e.upper - e.lower);
        balance[e.from] -= e.lower; balance[e.to] += e.lower;
    }
    long long demand = 0;
    for (int v = 0; v < n; ++v) {
        if (balance[v] > 0) { graph.add_edge(n, v, balance[v]); demand += balance[v]; }
        else if (balance[v] < 0) graph.add_edge(v, n + 1, -balance[v]);
    }
    if (graph.flow(n, n + 1, demand) != demand) return nullopt;
    vector<long long> flow;
    for (int i = 0; i < int(edges.size()); ++i)
        flow.push_back(edges[i].lower + graph.get_edge(i).flow);
    return flow;
}
}
