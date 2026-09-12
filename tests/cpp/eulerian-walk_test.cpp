#include "toolkit/eulerian_walk.hpp"
#include "test_util.hpp"

template<bool directed>
void check(int n, const vector<pair<int, int>>& edges) {
    ++cases_checked;
    string input = "n=" + to_string(n) + " directed=" + to_string(directed) + " edges=";
    for (auto [u, v] : edges) input += "(" + to_string(u) + "," + to_string(v) + ")";
    int m = edges.size();
    vector<vector<int>> memo(n, vector<int>(1 << m, -1));
    auto exists = [&](auto&& self, int v, int mask) -> bool {
        if (mask == (1 << m) - 1) return true;
        if (memo[v][mask] != -1) return memo[v][mask];
        bool answer = false;
        for (int i = 0; i < m; ++i) if (!(mask >> i & 1)) {
            auto [u, w] = edges[i];
            if (u == v) answer |= self(self, w, mask | (1 << i));
            else if (!directed && w == v) answer |= self(self, u, mask | (1 << i));
        }
        memo[v][mask] = answer;
        return answer;
    };
    bool possible = !m;
    for (int v = 0; v < n; ++v) possible |= exists(exists, v, 0);
    auto result = toolkit::eulerian_walk<directed>(n, edges);
    require(bool(result) == possible, input + " existence");
    if (!result) return;
    require(result->edges.size() == edges.size(), input);
    require(result->vertices.size() == (n ? edges.size() + 1 : 0), input);
    vector<bool> used(m);
    for (int i = 0; i < m; ++i) {
        int id = result->edges[i], a = result->vertices[i], b = result->vertices[i + 1];
        require(id >= 0 && id < m && !used[id], input + " edge identity");
        used[id] = true;
        auto [u, v] = edges[id];
        require((a == u && b == v) || (!directed && a == v && b == u), input + " orientation");
    }
    auto again = toolkit::eulerian_walk<directed>(n, edges);
    require(again && again->edges == result->edges && again->vertices == result->vertices,
            input + " repeat/no mutation");
}
int main() {
    for (int mask = 0; mask < 81; ++mask) {
        int code = mask;
        vector<pair<int, int>> edges;
        for (int u = 0; u < 2; ++u) for (int v = 0; v < 2; ++v) {
            int count = code % 3; code /= 3;
            while (count--) edges.push_back({u, v});
        }
        check<true>(2, edges); check<false>(2, edges);
    }
    for (int t = 0; t < 500; ++t) {
        int n = 1 + rng() % 5, m = rng() % 9;
        vector<pair<int, int>> edges;
        for (int i = 0; i < m; ++i) edges.push_back({rng() % n, rng() % n});
        check<true>(n, edges); check<false>(n, edges);
    }
    check<true>(0, {}); check<false>(0, {});
    check<true>(4, {{0, 0}, {2, 2}}); check<false>(4, {{0, 0}, {2, 2}});
    check<true>(5, {{3, 1}, {1, 4}}); check<false>(5, {{3, 1}, {1, 4}});
    for (int endpoint : {-1, 2}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::eulerian_walk<true>(2, {{0, endpoint}}); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 directed edge 0->" + to_string(endpoint));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::eulerian_walk<false>(-1, {}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "n=-1");
    success();
}
