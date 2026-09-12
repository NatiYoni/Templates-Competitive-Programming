#pragma once
#include "compression.hpp"
#include "misc/mo.hpp"

namespace toolkit {
// Unchanged Nyaan Mo ordering (CC0-1.0); original distinct-count integration.
// Static signed values; 0<=l<=r<=n half-open queries, empty ranges return zero.
// n,q<=10^7; invalid dimensions/ranges throw. Input unchanged, no globals.
// O(n log n+q log q+n sqrt(q+1)+q) time, O(n+q) space, sort O(log(n+q)) stack.
inline vector<int> mo_distinct(const vector<long long>& values,
                               const vector<pair<int, int>>& ranges) {
    if (values.size() > 10000000 || ranges.size() > 10000000)
        throw length_error("Mo dimensions");
    int n = int(values.size()), q = int(ranges.size());
    Mo mo(n, q);
    for (auto [l, r] : ranges) {
        if (l < 0 || l > r || r > n) throw out_of_range("Mo range");
        mo.insert(l, r);
    }
    CoordinateCompression<long long> compressed(values);
    vector<int> rank(n), frequency(compressed.values().size()), answer(q);
    for (int i = 0; i < n; ++i) rank[i] = int(compressed.rank(values[i]));
    int distinct = 0;
    auto add = [&](int i) { if (frequency[rank[i]]++ == 0) ++distinct; };
    auto remove = [&](int i) { if (--frequency[rank[i]] == 0) --distinct; };
    mo.run(add, add, remove, remove, [&](int i) { answer[i] = distinct; });
    return answer;
}
}
