#pragma once
#include "checked_tree.hpp"

namespace toolkit {
// Independent original ordered rerooting framework.
// merge must be associative with two-sided identity; commutativity is NOT needed.
// finish(aggregate,v) includes vertex v; transfer(state,from,to) crosses an edge.
// Neighbors are folded in input-edge insertion order, omitting the parent.
// O(n) callback invocations and T objects, iterative; T/callback cost is extra.
template<class T, class Merge, class Finish, class Transfer>
vector<T> rerooting(int n, const vector<pair<int, int>>& edges, const T& identity,
                   Merge merge, Finish finish, Transfer transfer) {
    CheckedTree tree(n, edges);
    vector<T> down(n, identity), up(n, identity), result(n, identity);
    for (auto it = tree.order.rbegin(); it != tree.order.rend(); ++it) {
        int v = *it;
        T fold = identity;
        for (int u : tree.adjacency[v]) if (u != tree.parent[v])
            fold = merge(fold, transfer(down[u], u, v));
        down[v] = finish(fold, v);
    }
    for (int v : tree.order) {
        const auto& adj = tree.adjacency[v];
        size_t degree = adj.size();
        vector<T> incoming(degree, identity), suffix(degree + 1, identity);
        for (size_t i = 0; i < degree; ++i) {
            int u = adj[i];
            incoming[i] = u == tree.parent[v] ? up[v] : transfer(down[u], u, v);
        }
        for (size_t i = degree; i > 0; --i)
            suffix[i - 1] = merge(incoming[i - 1], suffix[i]);
        result[v] = finish(suffix[0], v);
        T prefix = identity;
        for (size_t i = 0; i < degree; ++i) {
            int u = adj[i];
            if (u != tree.parent[v])
                up[u] = transfer(finish(merge(prefix, suffix[i + 1]), v), v, u);
            prefix = merge(prefix, incoming[i]);
        }
    }
    return result;
}
}
