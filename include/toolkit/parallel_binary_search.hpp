#pragma once
#include "base.hpp"

namespace toolkit {
// Earliest true prefix among 0..updates; updates+1 means never true.
// Each round calls reset(), then apply(i) exactly once in increasing order
// for i=0..updates-1. check(query) must be pure and false->true monotone.
// check sees exactly the first t updates at prefix t (including empty prefix).
// No calls when queries=0. State is left after the last round's full prefix.
// dimensions in [0,10^7]; callback exceptions propagate; no parallel threads.
// O((updates+queries)log(updates+2)) bookkeeping plus callback work,
// O(updates+queries) storage, no recursion.
template<class Reset, class Apply, class Check>
vector<int> parallel_binary_search(int updates, int queries,
                                   Reset reset, Apply apply, Check check) {
    if (updates < 0 || queries < 0 || updates > 10000000 || queries > 10000000)
        throw invalid_argument("parallel binary search dimensions");
    vector<int> low(queries, -1), high(queries, updates + 1);
    while (true) {
        vector<vector<int>> buckets(size_t(updates) + 1);
        bool unfinished = false;
        for (int q = 0; q < queries; ++q) if (high[q] - low[q] > 1) {
            int m = low[q] + (high[q] - low[q]) / 2;
            buckets[m].push_back(q);
            unfinished = true;
        }
        if (!unfinished) return high;
        reset();
        for (int t = 0; t <= updates; ++t) {
            if (t) apply(t - 1);
            for (int q : buckets[t]) {
                if (check(q)) high[q] = t;
                else low[q] = t;
            }
        }
    }
}
}
