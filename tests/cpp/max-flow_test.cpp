#include "test_util.hpp"
#include <atcoder/maxflow.hpp>

struct Edge { int u, v; long long capacity; };
// Independent oracle: enumerate all source-containing, sink-excluding cuts.
long long minimum_cut(int n, const vector<Edge>& edges) {
    long long result = LLONG_MAX;
    for (int mask = 0; mask < (1 << n); ++mask) {
        if (!(mask & 1) || (mask & (1 << (n - 1)))) continue;
        long long cut = 0;
        for (auto e : edges)
            if ((mask & (1 << e.u)) && !(mask & (1 << e.v))) cut += e.capacity;
        result = min(result, cut);
    }
    return result;
}
void check_flow(atcoder::mf_graph<long long>& graph, int n,
                const vector<Edge>& edges, long long total, const string& input) {
    auto actual = graph.edges();
    require(actual.size() == edges.size(), input + " edge count");
    vector<long long> balance(n);
    for (int i = 0; i < int(edges.size()); ++i) {
        auto x = actual[i], single = graph.get_edge(i);
        require(x.from == edges[i].u && x.to == edges[i].v && x.cap == edges[i].capacity,
                input + " edge metadata=" + to_string(i));
        require(0 <= x.flow && x.flow <= x.cap, input + " edge capacity=" + to_string(i));
        require(single.from == x.from && single.to == x.to && single.cap == x.cap && single.flow == x.flow,
                input + " get_edge=" + to_string(i));
        balance[x.from] -= x.flow;
        balance[x.to] += x.flow;
    }
    for (int v = 0; v < n; ++v)
        require(balance[v] == (v == 0 ? -total : v == n - 1 ? total : 0),
                input + " flow conservation=" + to_string(v));
}
void run_case(int n, const vector<Edge>& edges, long long limit) {
    ++cases_checked;
    string input = "n=" + to_string(n) + " s=0 t=" + to_string(n - 1) +
                   " limit=" + to_string(limit) + " edges=";
    atcoder::mf_graph<long long> graph(n), limited(n);
    for (int i = 0; i < int(edges.size()); ++i) {
        auto e = edges[i];
        input += "(" + to_string(e.u) + "," + to_string(e.v) + "," + to_string(e.capacity) + ")";
        int a = graph.add_edge(e.u, e.v, e.capacity);
        int b = limited.add_edge(e.u, e.v, e.capacity);
        require(a == i && b == i, input + " add_edge ID");
    }
    long long expected = minimum_cut(n, edges);
    long long full = graph.flow(0, n - 1);
    require(full == expected, input + " full flow expected=" + to_string(expected));
    check_flow(graph, n, edges, full, input);
    auto cut = graph.min_cut(0);
    require(cut[0] && !cut[n - 1], input + " residual source/sink separation");
    long long cut_capacity = 0;
    for (auto e : edges) if (cut[e.u] && !cut[e.v]) cut_capacity += e.capacity;
    require(cut_capacity == expected, input + " min_cut capacity");
    vector<bool> reachable(n);
    reachable[0] = true;
    auto actual = graph.edges();
    for (int pass = 0; pass < n; ++pass)
        for (auto e : actual) {
            if (reachable[e.from] && e.flow < e.cap) reachable[e.to] = true;
            if (reachable[e.to] && e.flow > 0) reachable[e.from] = true;
        }
    require(cut == reachable, input + " residual reachability");
    require(limited.flow(0, n - 1, 0) == 0, input + " zero limit");
    long long first = limited.flow(0, n - 1, limit);
    require(first == min(expected, limit), input + " limited flow");
    check_flow(limited, n, edges, first, input + " after limited call");
    long long rest = limited.flow(0, n - 1);
    require(first + rest == expected, input + " repeated augmentation");
    check_flow(limited, n, edges, first + rest, input + " after remainder");
    require(limited.flow(0, n - 1) == 0, input + " exhausted augmentation");
}
int main() {
    run_case(2, {}, 0);
    run_case(3, {{0, 0, 7}, {0, 1, 0}, {1, 2, 2}, {2, 2, 1}}, 4);
    run_case(2, {{0, 1, 3}, {0, 1, 4}, {1, 0, 8}}, 2);
    run_case(2, {{0, 1, 4000000000000000000LL}, {0, 1, 4000000000000000000LL}}, 1);
    for (int i = 0; i < 260; ++i) {
        int n = 2 + int(rng() % 5), m = int(rng() % 12);
        vector<Edge> edges;
        while (m--) edges.push_back({int(rng() % n), int(rng() % n), static_cast<long long>(rng() % 6)});
        run_case(n, edges, rng() % 12);
    }
    success();
}
