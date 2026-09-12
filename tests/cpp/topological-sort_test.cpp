#include "toolkit/topological_sort.hpp"
#include "test_util.hpp"

bool valid(const vector<vector<int>>& g, const vector<int>& order) {
    if (order.size() != g.size()) return false;
    vector<int> position(g.size(), -1);
    for (size_t i = 0; i < order.size(); ++i) {
        int v = order[i];
        if (v < 0 || size_t(v) >= g.size() || position[v] != -1) return false;
        position[v] = int(i);
    }
    for (size_t u = 0; u < g.size(); ++u)
        for (int v : g[u]) if (position[u] >= position[v]) return false;
    return true;
}

int main() {
    for (int n = 0; n <= 3; ++n)
        for (int mask = 0; mask < (1 << (n * n)); ++mask) {
            vector<vector<int>> g(n);
            for (int u = 0; u < n; ++u)
                for (int v = 0; v < n; ++v) if (mask >> (u * n + v) & 1) {
                    g[u].push_back(v);
                    if (u == 0) g[u].push_back(v);
                }
            vector<int> order(n);
            iota(order.begin(), order.end(), 0);
            bool possible = false;
            do { possible |= valid(g, order); } while (next_permutation(order.begin(), order.end()));
            ++cases_checked;
            auto result = toolkit::topological_sort(g);
            string input = "n=" + to_string(n) + " adjacency-bitmask=" + to_string(mask);
            require(bool(result) == possible, input);
            if (result) require(valid(g, *result), input);
            require(result == toolkit::topological_sort(g), input + " repeat");
        }
    ++cases_checked;
    require(toolkit::topological_sort({{3, 2}, {}, {}, {}})
            == optional<vector<int>>({0, 1, 3, 2}), "FIFO ties: edges 0->3,0->2; n=4");
    for (int to : {-1, 2}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::topological_sort({{}, {to}}); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 edge 1->" + to_string(to));
    }
    success();
}
