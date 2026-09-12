#pragma once
#include "base.hpp"

namespace toolkit {
struct SubsetOptimum { __int128 sum; unsigned long long mask; };

// Maximize subset sum <=limit for n<=40 signed-64 values, each used once.
// Empty subset allowed but only feasible if 0<=limit; nullopt means NO subset,
// not "empty wins". Return exact signed __int128 sum and input-index bit mask.
// Ties choose the numerically smallest mask. Full signed-64 input/limit is safe:
// every sum/difference has magnitude <=41*2^63, far within signed __int128.
// O(n*2^ceil(n/2)) time, O(2^ceil(n/2)) memory; unchanged input, no recursion.
inline optional<SubsetOptimum> meet_in_middle_subset(
    const vector<long long>& values, long long limit) {
    if (values.size() > 40) throw length_error("meet-in-middle supports at most 40 items");
    int n = int(values.size()), split = n / 2;
    using SumMask = pair<__int128, unsigned long long>;
    auto enumerate = [&](int first, int last) {
        vector<SumMask> sums{{0, 0}};
        sums.reserve(size_t(1) << (last - first));
        for (int i = first; i < last; ++i) {
            size_t count = sums.size();
            for (size_t j = 0; j < count; ++j)
                sums.push_back({sums[j].first + values[i], sums[j].second | (1ULL << i)});
        }
        return sums;
    };
    vector<SumMask> right = enumerate(split, n);
    sort(right.begin(), right.end());
    right.erase(unique(right.begin(), right.end(),
                       [](const SumMask& a, const SumMask& b) { return a.first == b.first; }),
                right.end());
    optional<SubsetOptimum> best;
    for (auto [sum, mask] : enumerate(0, split)) {
        __int128 remaining = (__int128)limit - sum;
        auto it = upper_bound(right.begin(), right.end(), remaining,
                              [](__int128 x, const SumMask& y) { return x < y.first; });
        if (it == right.begin()) continue;
        --it;
        SubsetOptimum candidate{sum + it->first, mask | it->second};
        if (!best || candidate.sum > best->sum ||
            (candidate.sum == best->sum && candidate.mask < best->mask))
            best = candidate;
    }
    return best;
}
}
