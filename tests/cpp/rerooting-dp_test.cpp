#include "toolkit/rerooting.hpp"
#include "test_util.hpp"

string describe(int n, const vector<pair<int, int>>& edges) {
    string s = "n=" + to_string(n) + " ordered-edges=";
    for (auto [u, v] : edges) s += "(" + to_string(u) + "," + to_string(v) + ")";
    return s;
}
auto merge_string = [](const string& a, const string& b) { return a + b; };
auto finish_string = [](const string& a, int v) { return "[" + to_string(v) + ":" + a + "]"; };
auto transfer_string = [](const string& a, int from, int to) {
    return "(" + to_string(from) + ">" + to_string(to) + a + ")";
};
void check(int n, const vector<pair<int, int>>& edges) {
    ++cases_checked;
    string input = describe(n, edges);
    auto actual = toolkit::rerooting(n, edges, string{}, merge_string, finish_string, transfer_string);
    using State = pair<long long, long long>;
    auto sums = toolkit::rerooting(n, edges, State{0, 0},
        [](State a, State b) { return State{a.first + b.first, a.second + b.second}; },
        [](State a, int) { return State{a.first + 1, a.second}; },
        [](State a, int, int) { return State{a.first, a.second + a.first}; });
    vector<vector<int>> g(n);
    for (auto [u, v] : edges) { g[u].push_back(v); g[v].push_back(u); }
    for (int root = 0; root < n; ++root) {
        // Independently root and serialize the whole ordered tree each time.
        auto serialize = [&](auto&& self, int v, int parent) -> string {
            string s;
            for (int u : g[v]) if (u != parent)
                s += "(" + to_string(u) + ">" + to_string(v) + self(self, u, v) + ")";
            return "[" + to_string(v) + ":" + s + "]";
        };
        require(actual[root] == serialize(serialize, root, -1), input + " noncommutative root=" + to_string(root));
        vector<int> d(n, -1), q{root}; d[root] = 0;
        for (size_t i = 0; i < q.size(); ++i) for (int u : g[q[i]]) if (d[u] == -1) {
            d[u] = d[q[i]] + 1; q.push_back(u);
        }
        require(sums[root] == State{n, accumulate(d.begin(), d.end(), 0LL)},
                input + " all-roots distance sum root=" + to_string(root));
    }
}
int main() {
    for (int t = 0; t < 500; ++t) {
        int n = 1 + rng() % 28;
        vector<int> label(n); iota(label.begin(), label.end(), 0);
        shuffle(label.begin(), label.end(), rng);
        vector<pair<int, int>> edges;
        for (int v = 1; v < n; ++v) {
            int p = t % 5 == 0 ? v - 1 : t % 5 == 1 ? 0 : rng() % v;
            edges.push_back({label[p], label[v]});
            if (rng() & 1) swap(edges.back().first, edges.back().second);
        }
        shuffle(edges.begin(), edges.end(), rng);
        check(n, edges);
    }
    check(5, {{2, 3}, {0, 2}, {2, 4}, {1, 2}});
    for (auto edges : vector<vector<pair<int, int>>>{
            {{0, 0}, {1, 2}}, {{0, 1}, {0, 1}}, {{0, 1}}, {{0, 1}, {1, 2}, {2, 0}}}) {
        int n = edges.size() == 3 ? 4 : 3;
        ++cases_checked; bool rejected = false;
        try { toolkit::rerooting(n, edges, string{}, merge_string, finish_string, transfer_string); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(n, edges));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::rerooting(0, {}, string{}, merge_string, finish_string, transfer_string); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "empty tree rejected");
    ++cases_checked; rejected = false;
    try { toolkit::rerooting(2, {{0, 2}}, string{}, merge_string, finish_string, transfer_string); }
    catch (const out_of_range&) { rejected = true; }
    require(rejected, "n=2 invalid endpoint=2");
    success();
}
