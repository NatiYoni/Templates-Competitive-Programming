#pragma once
#include "base.hpp"

namespace toolkit {
// Worked interval DP: optimally merge adjacent piles, paying the sum of each
// merged interval. dp[l][r]=sum(l,r)+min_{l<k<r}(dp[l][k]+dp[k][r]),
// with dp[l][l+1]=0. Empty/singleton cost is 0. Signed weights are supported.
// n<=500 checked; full signed-64 weights, exact signed __int128 costs
// (absolute cost <= (n-1)*sum(abs(weights))). O(n^3) time, O(n^2) space,
// no recursion/global state, input unchanged; not a generic interval solver.
inline __int128 adjacent_merge_cost(const vector<long long>& weights) {
    if (weights.size() > 500) throw length_error("cubic merge dimension");
    int n = int(weights.size());
    vector<__int128> prefix(n + 1);
    for (int i = 0; i < n; ++i) prefix[i + 1] = prefix[i] + weights[i];
    vector<vector<__int128>> dp(n + 1, vector<__int128>(n + 1));
    for (int len = 2; len <= n; ++len)
        for (int l = 0, r = len; r <= n; ++l, ++r) {
            __int128 best = dp[l][l + 1] + dp[l + 1][r];
            for (int k = l + 2; k < r; ++k)
                best = min(best, dp[l][k] + dp[k][r]);
            dp[l][r] = best + prefix[r] - prefix[l];
        }
    return dp[0][n];
}
}
