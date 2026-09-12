#include "toolkit/bfs.hpp"
#include "test_util.hpp"

int main() {
    for (int n = 1; n <= 3; ++n)
        for (int mask = 0; mask < (1 << (n * n)); ++mask) {
            vector<vector<int>> g(n), d(n, vector<int>(n, 100));
            for (int u = 0; u < n; ++u) {
                d[u][u] = 0;
                for (int v = 0; v < n; ++v) if (mask >> (u * n + v) & 1) {
                    g[u].push_back(v);
                    if (u + v == 1) g[u].push_back(v);
                    d[u][v] = min(d[u][v], 1);
                }
            }
            for (int k = 0; k < n; ++k)
                for (int u = 0; u < n; ++u)
                    for (int v = 0; v < n; ++v)
                        d[u][v] = min(d[u][v], d[u][k] + d[k][v]);
            for (int s = 0; s < n; ++s) {
                ++cases_checked;
                auto actual = toolkit::bfs(g, s);
                for (int v = 0; v < n; ++v)
                    require(actual[v] == (d[s][v] == 100 ? -1 : d[s][v]),
                            "n=" + to_string(n) + " adjacency-bitmask=" + to_string(mask)
                            + " source=" + to_string(s) + " vertex=" + to_string(v));
            }
        }
    auto reject = [](vector<vector<int>> g, int s, const string& input) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::bfs(g, s); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, input);
    };
    reject({}, 0, "empty graph source=0");
    reject({{}}, -1, "singleton source=-1");
    reject({{}}, 1, "singleton source=1");
    reject({{}, {-1}}, 0, "unreachable vertex 1 has edge -1");
    reject({{2}, {}}, 0, "n=2 edge 0->2");
    success();
}
