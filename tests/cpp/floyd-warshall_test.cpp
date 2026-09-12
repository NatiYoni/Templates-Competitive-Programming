#include "toolkit/floyd_warshall.hpp"
#include "test_util.hpp"
using toolkit::SignedEdge;
using toolkit::DistanceState;

string describe(int n, const vector<SignedEdge>& edges) {
    string s = "n=" + to_string(n) + " edges=";
    for (auto e : edges) s += "(" + to_string(e.from) + "," + to_string(e.to)
                               + "," + to_string(e.cost) + ")";
    return s;
}
void check(int n, const vector<SignedEdge>& edges) {
    ++cases_checked;
    string input = describe(n, edges);
    vector<vector<pair<int, long long>>> g(n);
    for (auto e : edges) g[e.from].push_back({e.to, e.cost});
    vector<vector<bool>> reach(n, vector<bool>(n));
    vector<bool> negative(n);
    vector<vector<optional<__int128>>> best(n, vector<optional<__int128>>(n));
    for (int root = 0; root < n; ++root) {
        auto dfs = [&](auto&& self, int v, int mask, __int128 cost) -> void {
            reach[root][v] = true;
            if (!best[root][v] || cost < *best[root][v]) best[root][v] = cost;
            for (auto [u, w] : g[v]) {
                if (u == root && cost + w < 0) negative[root] = true;
                if (!(mask >> u & 1)) self(self, u, mask | (1 << u), cost + w);
            }
        };
        dfs(dfs, root, 1 << root, 0);
    }
    auto actual = toolkit::floyd_warshall(n, edges);
    require(actual.size() == size_t(n), input);
    for (int s = 0; s < n; ++s) for (int v = 0; v < n; ++v) {
        bool bad = false;
        for (int k = 0; k < n; ++k) bad |= reach[s][k] && negative[k] && reach[k][v];
        auto state = bad ? DistanceState::negative_infinity
                   : reach[s][v] ? DistanceState::finite : DistanceState::unreachable;
        string context = input + " source=" + to_string(s) + " target=" + to_string(v);
        require(actual[s][v].state == state, context);
        require(bool(actual[s][v].value) == (state == DistanceState::finite), context + " value state");
        if (actual[s][v].value) require(__int128(*actual[s][v].value) == *best[s][v], context);
    }
}
int main() {
    for (int t = 0; t < 600; ++t) {
        int n = 1 + rng() % 6, m = rng() % 13;
        vector<SignedEdge> edges;
        for (int i = 0; i < m; ++i)
            edges.push_back({int(rng() % n), int(rng() % n), int(rng() % 15) - 7});
        check(n, edges);
    }
    check(0, {});
    check(5, {{0, 1, 4}, {1, 2, -2}, {2, 1, 1}, {2, 3, 8}, {4, 4, -1}});
    check(3, {{0, 0, -1}, {0, 1, 2}, {1, 1, 0}});
    long long b = (LLONG_MAX - 1) / 3;
    check(3, {{0, 1, b}, {1, 2, b}});
    check(3, {{0, 1, -b}, {1, 2, -b}, {2, 0, -b}});
    // Dense negative cycles exercise the internal floor without exposing it.
    ++cases_checked;
    vector<SignedEdge> dense;
    for (int u = 0; u < 80; ++u) for (int v = 0; v < 80; ++v) dense.push_back({u, v, -1});
    auto all_bad = toolkit::floyd_warshall(80, dense);
    for (const auto& row : all_bad) for (auto d : row)
        require(d.state == DistanceState::negative_infinity && !d.value, "n=80 all arcs cost=-1");
    for (auto edges : vector<vector<SignedEdge>>{{{0, 1, b + 1}}, {{2, 2, LLONG_MIN}}}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::floyd_warshall(3, edges); } catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(3, edges));
    }
    for (int endpoint : {-1, 3}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::floyd_warshall(3, {{2, endpoint, 0}}); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "edge 2->" + to_string(endpoint));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::floyd_warshall(-1, {}); } catch (const invalid_argument&) { rejected = true; }
    require(rejected, "n=-1");
    success();
}
