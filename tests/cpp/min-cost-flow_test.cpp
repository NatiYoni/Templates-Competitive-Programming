#include "test_util.hpp"
#include <atcoder/mincostflow.hpp>

struct Edge { int u, v, capacity; long long cost; };
// Independent oracle: enumerate every edge's integral flow in [0,capacity],
// retain conserved assignments, minimize total cost separately for each value.
vector<long long> enumerate_flows(int n, const vector<Edge>& edges) {
    int bound = 0;
    for (auto e : edges) if (e.u == 0) bound += e.capacity;
    vector<long long> best(bound + 1, LLONG_MAX), balance(n);
    auto visit = [&](auto&& self, int i, long long cost) -> void {
        if (i == int(edges.size())) {
            int value = int(-balance[0]);
            if (value < 0 || value > bound || balance[n - 1] != value) return;
            for (int v = 1; v + 1 < n; ++v) if (balance[v] != 0) return;
            best[value] = min(best[value], cost);
            return;
        }
        auto e = edges[i];
        for (int f = 0; f <= e.capacity; ++f) {
            balance[e.u] -= f;
            balance[e.v] += f;
            self(self, i + 1, cost + f * e.cost);
            balance[e.u] += f;
            balance[e.v] -= f;
        }
    };
    visit(visit, 0, 0);
    return best;
}
using FlowGraph = atcoder::mcf_graph<long long, long long>;
void check_edges(FlowGraph& graph, int n, const vector<Edge>& edges,
                 pair<long long, long long> result, const string& input) {
    auto actual = graph.edges();
    require(actual.size() == edges.size(), input + " edge count");
    vector<long long> balance(n);
    long long cost = 0;
    for (int i = 0; i < int(edges.size()); ++i) {
        auto x = actual[i], one = graph.get_edge(i);
        require(x.from == edges[i].u && x.to == edges[i].v &&
                x.cap == edges[i].capacity && x.cost == edges[i].cost,
                input + " metadata edge=" + to_string(i));
        require(0 <= x.flow && x.flow <= x.cap, input + " capacity edge=" + to_string(i));
        require(one.from == x.from && one.to == x.to && one.cap == x.cap &&
                one.flow == x.flow && one.cost == x.cost, input + " get_edge=" + to_string(i));
        balance[x.from] -= x.flow;
        balance[x.to] += x.flow;
        cost += x.flow * x.cost;
    }
    require(cost == result.second, input + " edge cost");
    for (int v = 0; v < n; ++v)
        require(balance[v] == (v == 0 ? -result.first : v == n - 1 ? result.first : 0),
                input + " conservation vertex=" + to_string(v));
}
void run_case(int n, const vector<Edge>& edges, int limit) {
    ++cases_checked;
    string input = "n=" + to_string(n) + " s=0 t=" + to_string(n - 1) +
                   " limit=" + to_string(limit) + " edges=";
    for (auto e : edges)
        input += "(" + to_string(e.u) + "," + to_string(e.v) + "," +
                 to_string(e.capacity) + "," + to_string(e.cost) + ")";
    auto best = enumerate_flows(n, edges);
    int maximum = 0;
    for (int f = 0; f < int(best.size()); ++f) if (best[f] != LLONG_MAX) maximum = f;
    int target = min(maximum, limit);
    auto make_graph = [&]() {
        FlowGraph graph(n);
        for (int i = 0; i < int(edges.size()); ++i) {
            auto e = edges[i];
            require(graph.add_edge(e.u, e.v, e.capacity, e.cost) == i, input + " add_edge ID");
        }
        return graph;
    };
    // Each graph receives exactly one solving call, as required by ACL.
    auto full = make_graph(), limited = make_graph(), curve_graph = make_graph();
    auto result = full.flow(0, n - 1);
    require(result == make_pair(static_cast<long long>(maximum), best[maximum]), input + " full optimum");
    check_edges(full, n, edges, result, input + " full");
    auto partial = limited.flow(0, n - 1, limit);
    require(partial == make_pair(static_cast<long long>(target), best[target]), input + " limited optimum");
    check_edges(limited, n, edges, partial, input + " limited");
    auto curve = curve_graph.slope(0, n - 1, limit);
    require(!curve.empty() && curve.front() == make_pair(0LL, 0LL) && curve.back() == partial,
            input + " slope endpoints");
    long long previous_slope = -1;
    for (int i = 1; i < int(curve.size()); ++i) {
        auto [left, left_cost] = curve[i - 1];
        auto [right, right_cost] = curve[i];
        require(left < right && left_cost <= right_cost, input + " slope monotonicity");
        require((right_cost - left_cost) % (right - left) == 0, input + " integral slope");
        long long rate = (right_cost - left_cost) / (right - left);
        require(rate > previous_slope, input + " merged collinear slope segments");
        previous_slope = rate;
        for (int f = int(left); f <= right; ++f)
            require(left_cost + (f - left) * rate == best[f],
                    input + " slope optimum flow=" + to_string(f));
    }
    check_edges(curve_graph, n, edges, partial, input + " slope");
}
int main() {
    run_case(2, {}, 0);
    run_case(2, {{0, 1, 2, 0}, {0, 1, 2, 0}}, 3);
    run_case(3, {{0, 0, 1, 0}, {0, 1, 0, 2}, {1, 2, 2, 3}}, 4);
    run_case(2, {{0, 1, 2, 1000000000000000LL}}, 2);
    // Second augmentation must cancel an earlier zero-cost choice.
    run_case(6, {{0, 1, 1, 0}, {0, 2, 1, 0}, {1, 3, 1, 0}, {1, 4, 1, 1},
                 {2, 3, 1, 1}, {3, 5, 1, 0}, {4, 5, 1, 0}}, 2);
    for (int i = 0; i < 260; ++i) {
        int n = 2 + int(rng() % 4), m = int(rng() % 9);
        vector<Edge> edges;
        while (m--) edges.push_back({int(rng() % n), int(rng() % n),
                                     int(rng() % 3), static_cast<long long>(rng() % 6)});
        run_case(n, edges, int(rng() % 7));
    }
    success();
}
