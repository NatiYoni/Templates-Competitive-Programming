#include "toolkit/lowlink.hpp"
#include "test_util.hpp"

string describe(int n, const vector<pair<int, int>>& edges) {
    string s = "n=" + to_string(n) + " edges=";
    for (auto [u, v] : edges) s += "(" + to_string(u) + "," + to_string(v) + ")";
    return s;
}
int components(int n, const vector<pair<int, int>>& edges, int skip_edge, int skip_vertex) {
    vector<vector<int>> g(n);
    for (int i = 0; i < int(edges.size()); ++i) {
        auto [u, v] = edges[i];
        if (i == skip_edge || u == skip_vertex || v == skip_vertex) continue;
        g[u].push_back(v); g[v].push_back(u);
    }
    vector<bool> seen(n);
    int count = 0;
    for (int root = 0; root < n; ++root) if (root != skip_vertex && !seen[root]) {
        ++count; vector<int> q{root}; seen[root] = true;
        for (size_t i = 0; i < q.size(); ++i)
            for (int u : g[q[i]]) if (!seen[u]) { seen[u] = true; q.push_back(u); }
    }
    return count;
}
void check(int n, const vector<pair<int, int>>& edges) {
    ++cases_checked;
    string input = describe(n, edges);
    auto actual = toolkit::lowlink(n, edges);
    int m = int(edges.size()), baseline = components(n, edges, -1, -1);
    vector<int> bridges, articulation;
    for (int i = 0; i < m; ++i)
        if (components(n, edges, i, -1) > baseline) bridges.push_back(i);
    for (int v = 0; v < n; ++v)
        if (components(n, edges, -1, v) > baseline) articulation.push_back(v);
    sort(actual.bridges.begin(), actual.bridges.end());
    sort(actual.articulation.begin(), actual.articulation.end());
    require(actual.bridges == bridges, input + " bridges");
    require(actual.articulation == articulation, input + " articulation");
    // Two edges are in one block iff connected through a chain of simple cycles.
    // Enumerate connected degree-two edge subsets (parallel 2-cycles included).
    vector<int> cls(m); iota(cls.begin(), cls.end(), 0);
    for (int mask = 1; mask < (1 << m); ++mask) {
        vector<int> degree(n);
        vector<vector<int>> g(n);
        int first_edge = -1, root = -1;
        bool loop = false;
        for (int i = 0; i < m; ++i) if (mask >> i & 1) {
            auto [u, v] = edges[i];
            loop |= u == v; ++degree[u]; ++degree[v];
            g[u].push_back(v); g[v].push_back(u);
            root = u; first_edge = i;
        }
        if (loop) continue;
        bool cycle = true;
        for (int d : degree) cycle &= d == 0 || d == 2;
        vector<bool> seen(n);
        vector<int> q{root}; seen[root] = true;
        for (size_t i = 0; i < q.size(); ++i)
            for (int u : g[q[i]]) if (!seen[u]) { seen[u] = true; q.push_back(u); }
        for (int v = 0; v < n; ++v) if (degree[v] && !seen[v]) cycle = false;
        if (!cycle) continue;
        int keep = cls[first_edge];
        for (int i = 0; i < m; ++i) if (mask >> i & 1) {
            int old = cls[i];
            for (int j = 0; j < m; ++j) if (cls[j] == old) cls[j] = keep;
        }
    }
    vector<vector<int>> expected;
    for (int i = 0; i < m; ++i) if (cls[i] == i) {
        vector<int> block;
        for (int j = 0; j < m; ++j) if (cls[j] == i) block.push_back(j);
        expected.push_back(block);
    }
    for (auto& block : actual.edge_blocks) sort(block.begin(), block.end());
    sort(expected.begin(), expected.end());
    sort(actual.edge_blocks.begin(), actual.edge_blocks.end());
    require(actual.edge_blocks == expected, input + " complete edge blocks");
}
int main() {
    for (int mask = 0; mask < 64; ++mask) {
        vector<pair<int, int>> edges;
        int bit = 0;
        for (int u = 0; u < 3; ++u) for (int v = u; v < 3; ++v, ++bit)
            if (mask >> bit & 1) edges.push_back({u, v});
        check(3, edges);
    }
    for (int t = 0; t < 600; ++t) {
        int n = 1 + rng() % 7, m = rng() % 10;
        vector<pair<int, int>> edges;
        for (int i = 0; i < m; ++i) edges.push_back({rng() % n, rng() % n});
        check(n, edges);
    }
    check(0, {});
    check(4, {{0, 1}, {1, 2}, {2, 0}, {1, 3}, {1, 3}, {1, 1}, {3, 3}});
    for (int endpoint : {-1, 2}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::lowlink(2, {{0, endpoint}}); } catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 endpoint=" + to_string(endpoint));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::lowlink(-1, {}); } catch (const invalid_argument&) { rejected = true; }
    require(rejected, "n=-1");
    success();
}
