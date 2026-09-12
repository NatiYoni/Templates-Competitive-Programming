#pragma once
#include "base.hpp"

namespace toolkit {
// Independently authored sparse point-assignment/range-sum persistence.
// Version 0 is n zeros; set branches from any live version and returns a new ID.
// n in [0,10^9], signed 64-bit leaves, exact signed __int128 sums; [l,r).
// IDs are instance-local, never deleted. Amortized O(log(n+1)) update time,
// O(log(n+1)) new nodes/query/stack, O(1 + updates*log(n+1)) space.
// A node budget includes the null node; budget/allocation failure leaves all
// versions unchanged. Invalid versions/indices throw out_of_range.
class PersistentRangeSum {
    struct Node { __int128 sum; size_t left, right; };
    int n_;
    size_t budget_;
    vector<Node> nodes_{{0, 0, 0}};
    vector<size_t> roots_{0};
    size_t assign(size_t root, int l, int r, int pos, long long value) {
        if (nodes_.size() >= budget_) throw length_error("persistent node budget");
        size_t id = nodes_.size();
        nodes_.push_back(nodes_[root]);
        if (r - l == 1) { nodes_[id].sum = value; return id; }
        int m = l + (r - l) / 2;
        if (pos < m) {
            size_t child = assign(nodes_[id].left, l, m, pos, value);
            nodes_[id].left = child;
        } else {
            size_t child = assign(nodes_[id].right, m, r, pos, value);
            nodes_[id].right = child;
        }
        nodes_[id].sum = nodes_[nodes_[id].left].sum + nodes_[nodes_[id].right].sum;
        return id;
    }
    __int128 query(size_t root, int l, int r, int a, int b) const {
        if (!root || r <= a || b <= l) return 0;
        if (a <= l && r <= b) return nodes_[root].sum;
        int m = l + (r - l) / 2;
        return query(nodes_[root].left, l, m, a, b) +
               query(nodes_[root].right, m, r, a, b);
    }
public:
    explicit PersistentRangeSum(int n, size_t max_nodes = 10000000)
        : n_(n), budget_(max_nodes) {
        if (n < 0 || n > 1000000000 || max_nodes == 0)
            throw invalid_argument("persistent dimensions/budget");
    }
    size_t versions() const { return roots_.size(); }
    size_t node_count() const { return nodes_.size(); }
    size_t set(size_t version, int pos, long long value) {
        if (version >= versions() || pos < 0 || pos >= n_)
            throw out_of_range("persistent version/point");
        struct Rollback {
            vector<Node>& nodes;
            size_t before;
            bool committed = false;
            ~Rollback() { if (!committed) nodes.resize(before); }
        } rollback{nodes_, nodes_.size()};
        size_t root = assign(roots_[version], 0, n_, pos, value);
        roots_.push_back(root);
        rollback.committed = true;
        return versions() - 1;
    }
    __int128 sum(size_t version, int l, int r) const {
        if (version >= versions() || l < 0 || l > r || r > n_)
            throw out_of_range("persistent version/range");
        return query(roots_[version], 0, n_, l, r);
    }
};
}
