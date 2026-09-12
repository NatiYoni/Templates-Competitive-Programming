#include "toolkit/lowlink.hpp"

// Edge IDs distinguish parallel edges; self-loops form singleton blocks.
// Output: 1 1 3 (bridges, articulation vertices, biconnected edge blocks).
int main() {
    auto result = toolkit::lowlink(3, {{0, 1}, {0, 1}, {1, 2}, {2, 2}});
    cout << result.bridges.size() << ' ' << result.articulation.size() << ' '
         << result.edge_blocks.size() << '\n';
}
