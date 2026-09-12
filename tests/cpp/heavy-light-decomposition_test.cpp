#include "toolkit/heavy_light.hpp"
#include "test_util.hpp"

string describe(int n, const vector<pair<int, int>>& edges, int root) {
    string s = "n=" + to_string(n) + " root=" + to_string(root) + " edges=";
    for (auto [u, v] : edges) s += "(" + to_string(u) + "," + to_string(v) + ")";
    return s;
}
void check(int n, const vector<pair<int, int>>& edges, int root) {
    ++cases_checked;
    string input = describe(n, edges, root);
    toolkit::HeavyLight hld(n, edges, root);
    vector<vector<int>> g(n);
    for (auto [u, v] : edges) { g[u].push_back(v); g[v].push_back(u); }
    vector<int> parent(n, -2), depth(n), q{root};
    parent[root] = -1;
    for (size_t i = 0; i < q.size(); ++i) for (int u : g[q[i]]) if (parent[u] == -2) {
        parent[u] = q[i]; depth[u] = depth[q[i]] + 1; q.push_back(u);
    }
    vector<bool> position_seen(n);
    for (int v = 0; v < n; ++v) {
        int p = hld.position()[v];
        require(p >= 0 && p < n && !position_seen[p] && hld.vertex_at()[p] == v, input);
        position_seen[p] = true;
        require(parent[v] == hld.parent()[v], input + " parent");
        for (bool edge : {false, true}) {
            auto [lo, hi] = hld.subtree(v, edge);
            vector<int> actual, expected;
            for (int i = lo; i < hi; ++i) actual.push_back(hld.vertex_at()[i]);
            for (int u = 0; u < n; ++u) {
                int x = u;
                while (x != -1 && x != v) x = parent[x];
                if (x == v && (!edge || u != v)) expected.push_back(u);
            }
            sort(actual.begin(), actual.end());
            require(actual == expected, input + " subtree=" + to_string(v) + " edge=" + to_string(edge));
        }
    }
    vector<long long> vertex_value(n), flat(n);
    for (int v = 0; v < n; ++v) flat[hld.position()[v]] = vertex_value[v] = v - 7;
    string operations = " updates(vertex-then-edge;u,v,delta)=";
    for (int trial = 0; trial < 100; ++trial) {
        int u = rng() % n, v = rng() % n, a = u, b = v;
        operations += "(" + to_string(u) + "," + to_string(v) + "," + to_string(trial) + ")";
        vector<int> left, right;
        while (a != b) {
            if (depth[a] >= depth[b]) { left.push_back(a); a = parent[a]; }
            else { right.push_back(b); b = parent[b]; }
        }
        left.push_back(a);
        left.insert(left.end(), right.rbegin(), right.rend());
        require(hld.lca(u, v) == a, input + " LCA");
        for (bool edge : {false, true}) {
            vector<int> expected = left, actual;
            if (edge) expected.erase(find(expected.begin(), expected.end(), a));
            auto segments = hld.path(u, v, edge);
            string context = input + operations + " path=" + to_string(u) + "->" + to_string(v)
                           + " edge=" + to_string(edge) + " trial=" + to_string(trial);
            long long got_sum = 0, expected_sum = 0;
            for (auto s : segments) {
                require(0 <= s.first && s.first < s.last && s.last <= n, context + " bounds");
                if (s.reversed) for (int i = s.last; i-- > s.first;)
                    actual.push_back(hld.vertex_at()[i]);
                else for (int i = s.first; i < s.last; ++i)
                    actual.push_back(hld.vertex_at()[i]);
                for (int i = s.first; i < s.last; ++i) { got_sum += flat[i]; flat[i] += trial; }
            }
            for (int x : expected) { expected_sum += vertex_value[x]; vertex_value[x] += trial; }
            require(actual == expected, context + " ordered vertices");
            require(got_sum == expected_sum, context + " dynamic path sum");
            int lg = 0;
            for (int x = n; x > 1; x /= 2) ++lg;
            require(segments.size() <= size_t(2 * lg + 1), context + " logarithmic segments");
        }
        auto [lo, hi] = hld.subtree(u);
        long long actual_sum = accumulate(flat.begin() + lo, flat.begin() + hi, 0LL), expected_sum = 0;
        for (int x = 0; x < n; ++x) {
            int y = x;
            while (y != -1 && y != u) y = parent[y];
            if (y == u) expected_sum += vertex_value[x];
        }
        require(actual_sum == expected_sum, input + operations + " subtree=" + to_string(u));
    }
}
int main() {
    for (int t = 0; t < 450; ++t) {
        int n = 1 + rng() % 60;
        vector<int> label(n); iota(label.begin(), label.end(), 0);
        shuffle(label.begin(), label.end(), rng);
        vector<pair<int, int>> edges;
        for (int v = 1; v < n; ++v) {
            int p = t % 5 == 0 ? v - 1 : t % 5 == 1 ? 0 : rng() % v;
            edges.push_back({label[p], label[v]});
            if (rng() & 1) swap(edges.back().first, edges.back().second);
        }
        shuffle(edges.begin(), edges.end(), rng);
        check(n, edges, rng() % n);
    }
    for (auto edges : vector<vector<pair<int, int>>>{
            {{0, 0}, {1, 2}}, {{0, 1}, {0, 1}}, {{0, 1}}, {{0, 1}, {1, 2}, {2, 0}}}) {
        int n = edges.size() == 3 ? 4 : 3;
        ++cases_checked; bool rejected = false;
        try { toolkit::HeavyLight hld(n, edges); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(n, edges, 0));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::HeavyLight hld(0, {}); } catch (const invalid_argument&) { rejected = true; }
    require(rejected, "empty tree rejected");
    ++cases_checked; rejected = false;
    try { toolkit::HeavyLight hld(2, {{0, 2}}); } catch (const out_of_range&) { rejected = true; }
    require(rejected, "tree endpoint outside n=2");
    ++cases_checked; rejected = false;
    try { toolkit::HeavyLight hld(1, {}, 1); } catch (const out_of_range&) { rejected = true; }
    require(rejected, "tree root=1 outside n=1");
    toolkit::HeavyLight singleton(1, {});
    for (int v : {-1, 1}) {
        ++cases_checked; rejected = false;
        try { singleton.path(0, v); } catch (const out_of_range&) { rejected = true; }
        require(rejected, "singleton invalid path target=" + to_string(v));
    }
    success();
}
