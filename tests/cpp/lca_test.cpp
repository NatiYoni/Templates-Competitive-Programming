#include "test_util.hpp"
#include "toolkit/kactl_prelude.hpp"
#include "content/graph/LCA.h"

// Independent oracle: mark all ancestors of a, then climb b's given parents.
int parent_lca(int a, int b, const vector<int>& parent) {
    vector<bool> marked(parent.size());
    for (int v = a; v != -1; v = parent[v]) marked[v] = true;
    while (!marked[b]) b = parent[b];
    return b;
}
void run_case(const vector<int>& parent, bool undirected) {
    ++cases_checked;
    int n = int(parent.size());
    vector<vi> graph(n);
    for (int v = 1; v < n; ++v) {
        graph[parent[v]].push_back(v);
        if (undirected) graph[v].push_back(parent[v]);
    }
    for (auto& neighbors : graph) shuffle(neighbors.begin(), neighbors.end(), rng);
    auto before = graph;
    string input = "root=0 parent=" + show(parent) + " undirected=" + to_string(undirected) + " adjacency=";
    for (const auto& row : graph) input += show(row);
    LCA ancestor(graph);
    require(graph == before, input + " input mutation");
    for (int a = 0; a < n; ++a)
        for (int b = 0; b < n; ++b)
            require(ancestor.lca(a, b) == parent_lca(a, b, parent),
                    input + " query=" + to_string(a) + "," + to_string(b));
}
int main() {
    for (bool undirected : {false, true}) {
        run_case({-1}, undirected);
        vector<int> chain(65), star(65, 0);
        chain[0] = star[0] = -1;
        for (int v = 1; v < 65; ++v) chain[v] = v - 1;
        run_case(chain, undirected);
        run_case(star, undirected);
    }
    for (int i = 0; i < 260; ++i) {
        int n = 1 + int(rng() % 38);
        vector<int> parent(n, -1), permutation(n);
        iota(permutation.begin(), permutation.end(), 0);
        shuffle(permutation.begin() + 1, permutation.end(), rng);
        for (int v = 1; v < n; ++v) parent[permutation[v]] = permutation[rng() % v];
        run_case(parent, bool(rng() & 1));
    }
    success();
}
