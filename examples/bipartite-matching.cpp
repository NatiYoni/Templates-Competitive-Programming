#include "toolkit/kactl_prelude.hpp"
#include "content/graph/HopcroftKarp.h"

// KACTL hopcroftKarp (Adam Soltan; CC0; dated 2026-01-13).
// g[u] contains right vertices in [0,R); u in [0,L). Initialize r(R,-1)
// BEFORE EACH call: existing matches are not a supported starting state.
// Both partitions may be empty (then no incident edges). Returns cardinality;
// r[v] is the matched left vertex or -1. Graph unchanged, r mutated.
// O((L+R+E)*sqrt(L+R+1)) time including initialization, O(L+R) workspace;
// recursive augmenting paths may use O(L) stack.
// Input g={{0},{1,2},{2}}, R=3. Output: 3 0 1 2
int main() {
    vector<vi> graph{{0}, {1, 2}, {2}};
    vi right_match(3, -1);
    cout << hopcroftKarp(graph, right_match);
    for (int left : right_match) cout << ' ' << left;
    cout << '\n';
}
