#pragma once
#include "base.hpp"

namespace toolkit {
// Exact minimum of inserted full lines on inclusive signed-64 integer [lo,hi].
// Full signed-64 slopes/intercepts/x are evaluated in signed __int128.
// Empty query returns nullopt; outside-domain queries throw. No deletion.
// O(log(domain size))<=65 time/stack, at most one node per insertion, O(lines)
// space. The node budget is checked conservatively before every insertion.
class DynamicLiChaoMin {
    static constexpr size_t none = numeric_limits<size_t>::max();
    struct Line {
        long long a, b;
        __int128 at(long long x) const { return (__int128)a * x + b; }
    };
    struct Node { Line line; size_t left = none, right = none; };
    long long lo_, hi_;
    size_t budget_;
    vector<Node> nodes_;
    static long long middle(long long l, long long r) {
        return (long long)((__int128)l + ((__int128)r - l) / 2);
    }
    void insert(size_t id, long long l, long long r, Line line) {
        long long m = middle(l, r);
        bool left_better = line.at(l) < nodes_[id].line.at(l);
        bool mid_better = line.at(m) < nodes_[id].line.at(m);
        if (mid_better) swap(line, nodes_[id].line);
        if (l == r) return;
        bool go_left = left_better != mid_better;
        size_t child = go_left ? nodes_[id].left : nodes_[id].right;
        if (child == none) {
            child = nodes_.size();
            nodes_.push_back({line});
            if (go_left) nodes_[id].left = child;
            else nodes_[id].right = child;
        } else {
            insert(child, go_left ? l : m + 1, go_left ? m : r, line);
        }
    }
public:
    DynamicLiChaoMin(long long lo, long long hi, size_t max_nodes = 1000000)
        : lo_(lo), hi_(hi), budget_(max_nodes) {
        if (lo > hi || max_nodes == 0) throw invalid_argument("Li Chao domain/budget");
    }
    void add(long long slope, long long intercept) {
        if (nodes_.size() >= budget_) throw length_error("Li Chao node budget");
        if (nodes_.size() == nodes_.capacity()) {
            size_t capacity = nodes_.capacity();
            size_t growth = min(budget_ - capacity, max(size_t(1), capacity));
            nodes_.reserve(capacity + growth); // Allocate before mutating any line.
        }
        if (nodes_.empty()) nodes_.push_back({{slope, intercept}});
        else insert(0, lo_, hi_, {slope, intercept});
    }
    optional<__int128> query(long long x) const {
        if (x < lo_ || x > hi_) throw out_of_range("Li Chao coordinate");
        if (nodes_.empty()) return nullopt;
        long long l = lo_, r = hi_;
        size_t id = 0;
        __int128 answer = nodes_[0].line.at(x);
        while (id != none) {
            answer = min(answer, nodes_[id].line.at(x));
            if (l == r) break;
            long long m = middle(l, r);
            if (x <= m) { id = nodes_[id].left; r = m; }
            else { id = nodes_[id].right; l = m + 1; }
        }
        return answer;
    }
};
}
