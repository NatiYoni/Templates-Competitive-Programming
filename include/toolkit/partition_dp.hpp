#pragma once
#include "fundamentals_arithmetic.hpp"
#include "dp/monotone-minima.hpp"

namespace toolkit {
// Partition n positions into exactly groups NONEMPTY contiguous segments.
// dp[g][j]=min_{g-1<=k<j}(dp[g-1][k]+cost(k,j)), dp[0][0]=0.
// Returns the last row indexed j=0..n; nullopt means infeasible.
// REQUIRE leftmost optimal cuts to be nondecreasing in j for EVERY layer;
// this is not checked/proved for an arbitrary callback. Cost must be pure,
// defined on 0<=l<r<=n, return long long, and take O(1) for the stated bound.
// Example valid cost: square of a nonnegative segment sum (Monge).
// Dimensions 0..10^6 checked, groups>n returns all-infeasible without callbacks.
// Checked additions throw if any visited candidate exceeds signed 64-bit.
// Uses unchanged Luzhiled monotone_minima_select (Unlicense).
// O(groups*n*log(n+1)) time, O(n) space, O(log(n+1)) recursion.
template<class Cost>
vector<optional<long long>> partition_dp(int n, int groups, Cost cost) {
    if (n < 0 || groups < 0 || n > 1000000 || groups > 1000000)
        throw invalid_argument("partition dimensions");
    vector<optional<long long>> previous(size_t(n) + 1);
    if (groups > n) return previous;
    previous[0] = 0;
    for (int g = 1; g <= groups; ++g) {
        vector<optional<long long>> current(size_t(n) + 1);
        int width = n - g + 1;
        monotone_minima_select(width, width, [&](int row, int left, int right) {
            int j = row + g, best_column = -1;
            optional<long long> best;
            for (int column = left; column < right && column <= row; ++column) {
                int k = column + g - 1;
                if (!previous[k]) continue;
                long long candidate = fundamentals_detail::checked_add(*previous[k], cost(k, j));
                if (!best || candidate < *best) {
                    best = candidate;
                    best_column = column;
                }
            }
            if (best_column < 0) throw logic_error("violated monotone partition contract");
            current[j] = best;
            return best_column;
        });
        previous.swap(current);
    }
    return previous;
}
}
