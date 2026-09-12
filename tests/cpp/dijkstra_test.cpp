#include "toolkit/dijkstra.hpp"
#include "test_util.hpp"

int main() {
    for (int trial = 0; trial < 300; ++trial) {
        int n = 1 + rng() % 6;
        vector<vector<pair<int, long long>>> g(n);
        vector<tuple<int, int, long long>> edges;
        ostringstream input;
        input << "n=" << n << " edges=";
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                for (int rep = 0; rep < 2; ++rep) if (rng() % 4 == 0) {
                    long long w = rng() % 21;
                    g[u].push_back({v, w});
                    edges.emplace_back(u, v, w);
                    input << '(' << u << ',' << v << ',' << w << ')';
                }
        for (int s = 0; s < n; ++s) {
            ++cases_checked;
            vector<long long> d(n, LLONG_MAX);
            d[s] = 0;
            for (int pass = 1; pass < n; ++pass)
                for (auto [u, v, w] : edges)
                    if (d[u] != LLONG_MAX) d[v] = min(d[v], d[u] + w);
            require(toolkit::dijkstra(g, s) == d, input.str() + " source=" + to_string(s));
        }
    }
    long long bound = (LLONG_MAX - 1) / 3;
    ++cases_checked;
    require(toolkit::dijkstra({{{1, bound}}, {{2, bound}}, {}}, 0)
            == vector<long long>{0, bound, 2 * bound}, "n=3 chain weights=" + to_string(bound));
    ++cases_checked;
    require(toolkit::dijkstra({{{0, LLONG_MAX - 1}}}, 0) == vector<long long>{0},
            "n=1 maximum permitted self-loop");
    for (long long weight : {-1LL, (LLONG_MAX - 1) / 2 + 1, LLONG_MAX}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::dijkstra({{}, {{1, weight}}}, 0); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, "n=2 unreachable edge 1->1 weight=" + to_string(weight));
    }
    for (int to : {-1, 2}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::dijkstra({{}, {{to, 0}}}, 0); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 unreachable edge 1->" + to_string(to));
    }
    for (int s : {-1, 1}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::dijkstra({{}}, s); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=1 source=" + to_string(s));
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::dijkstra({}, 0); }
    catch (const out_of_range&) { rejected = true; }
    require(rejected, "n=0 source=0");
    success();
}
