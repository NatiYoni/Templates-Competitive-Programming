#include "toolkit/base.hpp"
#include <atcoder/scc.hpp>

// ACL scc_graph (CC0), directed vertices [0,n), n >= 0; parallel/self edges OK.
// scc() returns vertex groups in condensation topological order; order within
// a group is not part of this example's contract. O(n+m) time/space.
// Recursive Tarjan DFS can exhaust the stack on long directed paths.
// Input edges 0->1,1->0,1->2,2->3,3->2. Output: 0 0 1 1
int main() {
    atcoder::scc_graph graph(4);
    for (auto [u, v] : vector<pair<int, int>>{{0, 1}, {1, 0}, {1, 2}, {2, 3}, {3, 2}})
        graph.add_edge(u, v);
    auto groups = graph.scc();
    vector<int> component(4);
    for (int i = 0; i < int(groups.size()); ++i)
        for (int v : groups[i]) component[v] = i;
    for (int v = 0; v < 4; ++v) cout << (v ? " " : "") << component[v];
    cout << '\n';
}
