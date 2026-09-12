#include "toolkit/kruskal.hpp"

// Input (fixed): n=4; undirected (0,1,5),(1,2,-2),(0,2,1), vertex 3 isolated.
// Expected output: weight= -1, components=2, selected input indices=1 2.
// -1 2
// 1 2
int main() {
    auto forest = toolkit::kruskal(4, {{0, 1, 5}, {1, 2, -2}, {0, 2, 1}});
    cout << forest.weight << ' ' << forest.components << '\n';
    for (size_t i = 0; i < forest.edges.size(); ++i)
        cout << (i ? " " : "") << forest.edges[i];
    cout << '\n';
}
