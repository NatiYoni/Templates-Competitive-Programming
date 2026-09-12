#pragma once
#include "kactl_prelude.hpp"
#include "content/data-structures/UnionFindRollback.h"

namespace toolkit {
// Local timeline integration of unchanged KACTL RollbackUF:
// Lukas Polacek, Simon Lindholm; CC0; Source: folklore.
enum class ConnectivityAction { add, remove, query };
struct ConnectivityEvent { ConnectivityAction action; int u, v; };

// Undirected multigraph, initially empty; add/remove one copy, query connectivity.
// Endpoints normalized, loops allowed; removing an absent copy is invalid.
// n<=10^8, events<=10^6; valid endpoints [0,n), results in query order.
// O((n + q log(q+1) log(n+1))) time, O(n+q log(q+1)) space.
// Input unchanged; local rollback traversal has O(log(q+1)+log(n+1)) stack.
inline vector<bool> offline_connectivity(int n, const vector<ConnectivityEvent>& events) {
    if (n < 0 || n > 100000000 || events.size() > 1000000)
        throw invalid_argument("connectivity dimensions");
    int q = int(events.size());
    if (!q) return {};
    using Edge = pair<int, int>;
    vector<vector<Edge>> tree(size_t(q) * 4);
    map<Edge, pair<int, int>> active;  // count, start of the nonzero interval
    auto interval = [&](auto&& self, int id, int l, int r, int a, int b, Edge e) -> void {
        if (r <= a || b <= l) return;
        if (a <= l && r <= b) { tree[id].push_back(e); return; }
        int m = l + (r - l) / 2;
        self(self, id * 2, l, m, a, b, e);
        self(self, id * 2 + 1, m, r, a, b, e);
    };
    for (int t = 0; t < q; ++t) {
        auto event = events[t];
        if (event.u < 0 || event.v < 0 || event.u >= n || event.v >= n)
            throw out_of_range("connectivity vertex at event " + to_string(t));
        Edge e = minmax(event.u, event.v);
        if (event.action == ConnectivityAction::add) {
            auto& state = active[e];
            if (state.first++ == 0) state.second = t;
        } else if (event.action == ConnectivityAction::remove) {
            auto it = active.find(e);
            if (it == active.end() || it->second.first == 0)
                throw invalid_argument("removing absent edge at event " + to_string(t));
            if (--it->second.first == 0)
                interval(interval, 1, 0, q, it->second.second, t, e);
        } else if (event.action != ConnectivityAction::query) {
            throw invalid_argument("unknown connectivity action");
        }
    }
    for (auto [edge, state] : active)
        if (state.first) interval(interval, 1, 0, q, state.second, q, edge);
    RollbackUF uf(n);
    vector<bool> answer;
    auto visit = [&](auto&& self, int id, int l, int r) -> void {
        int snapshot = uf.time();
        for (auto [u, v] : tree[id]) uf.join(u, v);
        if (r - l == 1) {
            if (events[l].action == ConnectivityAction::query)
                answer.push_back(uf.find(events[l].u) == uf.find(events[l].v));
        } else {
            int m = l + (r - l) / 2;
            self(self, id * 2, l, m);
            self(self, id * 2 + 1, m, r);
        }
        uf.rollback(snapshot);
    };
    visit(visit, 1, 0, q);
    return answer;
}
}
