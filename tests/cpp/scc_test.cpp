#include "test_util.hpp"
#include <atcoder/scc.hpp>

// Independent oracle: Floyd-Warshall reachability, then mutual reachability.
void run_case(int n, const vector<pair<int, int>>& edges) {
    ++cases_checked;
    atcoder::scc_graph graph(n);
    vector<vector<bool>> reachable(n, vector<bool>(n));
    string input = "n=" + to_string(n) + " edges=";
    for (int v = 0; v < n; ++v) reachable[v][v] = true;
    for (auto [u, v] : edges) {
        graph.add_edge(u, v);
        reachable[u][v] = true;
        input += "(" + to_string(u) + "," + to_string(v) + ")";
    }
    for (int k = 0; k < n; ++k)
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                reachable[u][v] = reachable[u][v] || (reachable[u][k] && reachable[k][v]);
    auto groups = graph.scc();
    vector<int> component(n, -1);
    for (int i = 0; i < int(groups.size()); ++i) {
        require(!groups[i].empty(), input + " empty SCC=" + to_string(i));
        for (int v : groups[i]) {
            require(0 <= v && v < n, input + " invalid SCC vertex=" + to_string(v));
            require(component[v] == -1, input + " repeated vertex=" + to_string(v));
            component[v] = i;
        }
    }
    for (int u = 0; u < n; ++u) {
        require(component[u] != -1, input + " missing vertex=" + to_string(u));
        for (int v = 0; v < n; ++v)
            require((component[u] == component[v]) == (reachable[u][v] && reachable[v][u]),
                    input + " pair=" + to_string(u) + "," + to_string(v));
    }
    for (auto [u, v] : edges)
        require(component[u] <= component[v], input + " component topological order");
    require(graph.scc() == groups, input + " repeated solve");
}
int main() {
    run_case(0, {});
    run_case(1, {});
    run_case(4, {{0, 1}, {0, 1}, {1, 0}, {1, 2}, {2, 3}, {3, 2}});
    for (int mask = 0; mask < (1 << 9); ++mask) {
        vector<pair<int, int>> edges;
        for (int u = 0; u < 3; ++u)
            for (int v = 0; v < 3; ++v)
                if (mask & (1 << (3 * u + v))) edges.push_back({u, v});
        run_case(3, edges);
    }
    for (int i = 0; i < 260; ++i) {
        int n = int(rng() % 9), m = n ? int(rng() % 24) : 0;
        vector<pair<int, int>> edges;
        while (m--) edges.push_back({int(rng() % n), int(rng() % n)});
        run_case(n, edges);
    }
    success();
}
