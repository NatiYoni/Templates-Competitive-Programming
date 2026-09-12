#include "toolkit/kactl_prelude.hpp"
#include "content/graph/LCA.h"

// KACTL LCA (chilli, pajenegod; CC0; Source: Folklore), depends on RMQ.h
// (Johan Sannemo, pajenegod; CC0; Source: Folklore).
// Nonempty valid tree rooted at 0: undirected adjacency or parent->child edges.
// No cycles, duplicate tree edges, disconnected vertices, or arbitrary root.
// Build O(n log n), queries O(1), space O(n log n); recursive DFS depth O(n)
// on chains can exceed stack limits. Singleton lca(0,0) is valid.
// Input parents [-1,0,0,1,1]. Output: 1 0
int main() {
    vector<vi> tree{{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    LCA ancestor(tree);
    cout << ancestor.lca(3, 4) << ' ' << ancestor.lca(2, 4) << '\n';
}
