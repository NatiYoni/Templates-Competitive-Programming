#include "toolkit/lower_bound_circulation.hpp"
#include "test_util.hpp"
using toolkit::BoundedEdge;

string describe(int n, const vector<BoundedEdge>& edges) {
    string s = "n=" + to_string(n) + " edges=";
    for (auto e : edges) s += "(" + to_string(e.from) + "," + to_string(e.to) + ","
                               + to_string(e.lower) + "," + to_string(e.upper) + ")";
    return s;
}
void verify(int n, const vector<BoundedEdge>& edges,
            const optional<vector<long long>>& flow, const string& input) {
    require(bool(flow), input + " expected feasible");
    require(flow->size() == edges.size(), input + " flow size");
    vector<__int128> balance(n);
    for (int i = 0; i < int(edges.size()); ++i) {
        auto e = edges[i]; long long f = (*flow)[i];
        require(e.lower <= f && f <= e.upper, input + " bounds edge=" + to_string(i));
        balance[e.from] -= f; balance[e.to] += f;
    }
    for (auto b : balance) require(b == 0, input + " conservation");
}
void check(int n, const vector<BoundedEdge>& edges) {
    ++cases_checked;
    string input = describe(n, edges);
    vector<long long> balance(n);
    auto enumerate = [&](auto&& self, int i) -> bool {
        if (i == int(edges.size()))
            return all_of(balance.begin(), balance.end(), [](long long b) { return !b; });
        auto e = edges[i];
        for (long long f = e.lower; f <= e.upper; ++f) {
            balance[e.from] -= f; balance[e.to] += f;
            bool yes = self(self, i + 1);
            balance[e.from] += f; balance[e.to] -= f;
            if (yes) return true;
        }
        return false;
    };
    bool possible = enumerate(enumerate, 0);
    auto result = toolkit::lower_bound_circulation(n, edges);
    require(bool(result) == possible, input + " exhaustive feasibility");
    if (possible) verify(n, edges, result, input);
}
int main() {
    for (int t = 0; t < 700; ++t) {
        int n = 1 + rng() % 5, m = rng() % 8;
        vector<BoundedEdge> edges;
        for (int i = 0; i < m; ++i) {
            int upper = rng() % 4, lower = rng() % (upper + 1);
            edges.push_back({int(rng() % n), int(rng() % n), lower, upper});
        }
        check(n, edges);
    }
    check(0, {});
    check(4, {{0, 1, 1, 2}, {1, 0, 0, 2}, {2, 3, 1, 1}});
    check(2, {{0, 1, 2, 2}, {1, 0, 1, 1}, {1, 0, 1, 1}, {0, 0, 2, 3}});
    for (auto edges : vector<vector<BoundedEdge>>{
            {{0, 1, LLONG_MAX, LLONG_MAX}, {1, 0, 0, LLONG_MAX}},
            {{0, 0, LLONG_MAX, LLONG_MAX}},
            {{0, 1, 0, LLONG_MAX}, {1, 0, 0, LLONG_MAX}}}) {
        ++cases_checked;
        verify(2, edges, toolkit::lower_bound_circulation(2, edges), describe(2, edges));
    }
    for (auto edges : vector<vector<BoundedEdge>>{
            {{0, 1, -1, 2}}, {{0, 1, 2, 1}},
            {{0, 0, LLONG_MAX, LLONG_MAX}, {1, 1, 1, 1}}}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::lower_bound_circulation(2, edges); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(2, edges));
    }
    for (int endpoint : {-1, 2}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::lower_bound_circulation(2, {{0, endpoint, 0, 1}}); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 endpoint=" + to_string(endpoint));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::lower_bound_circulation(-1, {}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "n=-1");
    success();
}
