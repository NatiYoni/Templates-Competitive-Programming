#include "toolkit/dag_shortest_paths.hpp"
#include "test_util.hpp"
using toolkit::SignedEdge;

string describe(int n, const vector<SignedEdge>& edges, int source) {
    string s = "n=" + to_string(n) + " source=" + to_string(source) + " edges=";
    for (auto e : edges) s += "(" + to_string(e.from) + "," + to_string(e.to)
                               + "," + to_string(e.cost) + ")";
    return s;
}
void check(int n, const vector<SignedEdge>& edges, int source) {
    ++cases_checked;
    string input = describe(n, edges, source);
    vector<int> order(n), pos(n);
    iota(order.begin(), order.end(), 0);
    bool dag = false;
    do {
        for (int i = 0; i < n; ++i) pos[order[i]] = i;
        bool valid = true;
        for (auto e : edges) valid &= pos[e.from] < pos[e.to];
        if (valid) { dag = true; break; }
    } while (next_permutation(order.begin(), order.end()));
    auto actual = toolkit::dag_shortest_paths(n, edges, source);
    require(bool(actual) == dag, input + " permutation cycle oracle");
    if (!dag) return;
    vector<optional<__int128>> best(n);
    auto dfs = [&](auto&& self, int v, __int128 cost) -> void {
        if (!best[v] || cost < *best[v]) best[v] = cost;
        for (auto e : edges) if (e.from == v) self(self, e.to, cost + e.cost);
    };
    dfs(dfs, source, 0);
    for (int v = 0; v < n; ++v) {
        require(bool((*actual)[v].value) == bool(best[v]), input);
        require((*actual)[v].state == (best[v] ? toolkit::DistanceState::finite
                                              : toolkit::DistanceState::unreachable), input);
        if (best[v]) require(__int128(*(*actual)[v].value) == *best[v], input);
    }
}
int main() {
    for (int t = 0; t < 700; ++t) {
        int n = 1 + rng() % 6, m = rng() % 14;
        vector<int> label(n); iota(label.begin(), label.end(), 0);
        shuffle(label.begin(), label.end(), rng);
        vector<SignedEdge> edges;
        for (int i = 0; i < m; ++i) {
            int u = rng() % n, v = rng() % n;
            if (t % 2 && u >= v) continue;
            edges.push_back({label[u], label[v], int(rng() % 21) - 10});
        }
        check(n, edges, rng() % n);
    }
    check(4, {{2, 3, -1}, {3, 2, 0}}, 0);
    check(2, {{1, 1, 0}}, 0);
    check(3, {{0, 1, 0}, {0, 1, -5}, {1, 2, 2}}, 0);
    long long b = (LLONG_MAX - 1) / 3;
    check(3, {{0, 1, -b}, {1, 2, -b}}, 0);
    check(3, {{0, 1, b}, {1, 2, b}}, 0);
    for (auto e : vector<SignedEdge>{{2, 2, LLONG_MIN}, {2, 2, b + 1}}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::dag_shortest_paths(3, {e}, 0); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(3, {e}, 0));
    }
    for (int source : {-1, 3}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::dag_shortest_paths(3, {}, source); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, describe(3, {}, source));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::dag_shortest_paths(3, {{2, 3, 0}}, 0); }
    catch (const out_of_range&) { rejected = true; }
    require(rejected, "n=3 unreachable invalid edge 2->3");
    ++cases_checked; rejected = false;
    try { toolkit::dag_shortest_paths(0, {}, 0); } catch (const out_of_range&) { rejected = true; }
    require(rejected, "empty graph source=0");
    success();
}
