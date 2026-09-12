#include "toolkit/base.hpp"
#include <atcoder/maxflow.hpp>

// ACL mf_graph<long long> (CC0). Directed vertices [0,n), distinct s,t,
// nonnegative capacities and limit; total flow fits long long.
// flow mutates residual state and returns additional flow on repeated calls.
// min_cut(s) certifies a cut only after exhausting s->t augmentation, not merely
// reaching a limit. Recursive Dinic: O(n^2 m) general time, O(n+m) space.
// Input two disjoint paths of capacities 3 and 2. Output: 5 3 1 0
int main() {
    atcoder::mf_graph<long long> graph(4);
    int first = graph.add_edge(0, 1, 3);
    graph.add_edge(1, 3, 3);
    graph.add_edge(0, 2, 2);
    graph.add_edge(2, 3, 2);
    cout << graph.flow(0, 3) << ' ' << graph.get_edge(first).flow;
    auto cut = graph.min_cut(0);
    cout << ' ' << cut[0] << ' ' << cut[3] << '\n';
}
