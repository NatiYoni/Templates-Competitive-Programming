#pragma once
#include "base.hpp"

namespace toolkit {
// Optimal adjacent merges for NONNEGATIVE signed-64 weights, n<=3000.
// w(l,r)=sum(weights[l..r]) is interval-monotone and satisfies the quadrangle
// inequality with equality; hence leftmost opt[l][r-1]<=opt[l][r]<=opt[l+1][r].
// These conditions justify restricting the split search; negative weights
// are rejected, not silently fed to an unjustified optimization.
// Inclusive DP intervals: dp[l][l]=0, dp[l][r]=w(l,r)+
// min_{l<=k<r}(dp[l][k]+dp[k+1][r]). Empty cost is 0.
// O(n^2) time/space, signed __int128 exact costs bounded by
// (n-1)*sum(weights). No recursion/globals; input unchanged.
inline __int128 knuth_adjacent_merge_cost(const vector<long long>& weights) {
    if (weights.size() > 3000) throw length_error("Knuth merge dimension");
    for (long long x : weights) if (x < 0) throw invalid_argument("negative Knuth weight");
    int n = int(weights.size());
    if (!n) return 0;
    vector<__int128> prefix(n + 1);
    for (int i = 0; i < n; ++i) prefix[i + 1] = prefix[i] + weights[i];
    vector<vector<__int128>> dp(n, vector<__int128>(n));
    vector<vector<int>> opt(n, vector<int>(n));
    for (int i = 0; i < n; ++i) opt[i][i] = i;
    for (int len = 2; len <= n; ++len)
        for (int l = 0, r = len - 1; r < n; ++l, ++r) {
            int begin = opt[l][r - 1], end = min(r - 1, opt[l + 1][r]);
            int best = begin;
            __int128 value = dp[l][begin] + dp[begin + 1][r];
            for (int k = begin + 1; k <= end; ++k) {
                __int128 candidate = dp[l][k] + dp[k + 1][r];
                if (candidate < value) { value = candidate; best = k; }
            }
            opt[l][r] = best;
            dp[l][r] = value + prefix[r + 1] - prefix[l];
        }
    return dp[0][n - 1];
}
}
