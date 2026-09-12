#include "toolkit/zero_one_bfs.hpp"
#include "test_util.hpp"

int main() {
    for (int mask = 0; mask < 729; ++mask) {
        vector<vector<pair<int, int>>> g(3);
        vector<vector<int>> d(3, vector<int>(3, 100));
        int x = mask;
        for (int u = 0; u < 3; ++u) {
            d[u][u] = 0;
            g[u].push_back({u, 0});
            for (int v = 0; v < 3; ++v) if (u != v) {
                int state = x % 3; x /= 3;
                if (state) {
                    g[u].push_back({v, state - 1});
                    if (u == 0) g[u].push_back({v, 1});
                    d[u][v] = state - 1;
                }
            }
        }
        for (int k = 0; k < 3; ++k)
            for (int u = 0; u < 3; ++u)
                for (int v = 0; v < 3; ++v)
                    d[u][v] = min(d[u][v], d[u][k] + d[k][v]);
        for (int s = 0; s < 3; ++s) {
            ++cases_checked;
            auto actual = toolkit::zero_one_bfs(g, s);
            for (int v = 0; v < 3; ++v)
                require(actual[v] == (d[s][v] == 100 ? LLONG_MAX : d[s][v]),
                        "n=3 off-diagonal ternary-mask=" + to_string(mask)
                        + " source=" + to_string(s) + " vertex=" + to_string(v));
        }
    }
    for (int weight : {-1, 2, INT_MAX}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::zero_one_bfs({{}, {{1, weight}}}, 0); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, "unreachable edge 1->1 weight=" + to_string(weight));
    }
    for (int to : {-1, 2}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::zero_one_bfs({{}, {{to, 0}}}, 0); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 unreachable edge 1->" + to_string(to));
    }
    for (int source : {-1, 1}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::zero_one_bfs({{}}, source); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=1 source=" + to_string(source));
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::zero_one_bfs({}, 0); }
    catch (const out_of_range&) { rejected = true; }
    require(rejected, "empty graph source=0");
    success();
}
