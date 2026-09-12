#include "toolkit/base.hpp"
#include <atcoder/mincostflow.hpp>

// ACL mcf_graph<long long,long long> (CC0). Directed [0,n), distinct s,t;
// original capacities/costs and limit nonnegative. Exactly ONE flow OR slope
// call per graph: rebuild for another solve (reverse residual costs can be
// negative). Total flow/cost fit long long; n*max_cost <= 8e18+1000 per ACL.
// O(F(n+m)log(n+m)) time, O(n+m) space, no graph-depth recursion.
// Input: two units through costs 1+2, one through costs 0+5. Output: 3 11 2
int main() {
    atcoder::mcf_graph<long long, long long> graph(4);
    int first = graph.add_edge(0, 1, 2, 1);
    graph.add_edge(1, 3, 2, 2);
    graph.add_edge(0, 2, 1, 0);
    graph.add_edge(2, 3, 1, 5);
    auto [flow, cost] = graph.flow(0, 3, 3);
    cout << flow << ' ' << cost << ' ' << graph.get_edge(first).flow << '\n';
}
