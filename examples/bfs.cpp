#include "toolkit/bfs.hpp"

// Input (fixed): 0->1, 1->2, vertex 3 isolated; source 0. Expected: 0 1 2 -1
int main() {
    auto distance = toolkit::bfs({{1}, {2}, {}, {}}, 0);
    for (size_t i = 0; i < distance.size(); ++i) cout << (i ? " " : "") << distance[i];
    cout << '\n';
}
