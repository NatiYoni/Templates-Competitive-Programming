#pragma once
#include "base.hpp"

namespace toolkit {
// Repository-original full-table DP and reconstruction; not an upstream copy.
// Byte sequences, including NUL; returns one actual LCS, not necessarily unique
// or lexicographically least. O(n*m) time/space, no input mutation or recursion.
inline string longest_common_subsequence(const string& a, const string& b) {
    size_t n = a.size(), m = b.size();
    if (n >= size_t(INT_MAX) || m >= size_t(INT_MAX))
        throw length_error("LCS length bound");
    if (n == 0 || m == 0) return {};
    size_t stride = m + 1;
    if (n + 1 > vector<int>().max_size() / stride)
        throw length_error("LCS table size");
    vector<int> dp((n + 1) * stride);
    auto at = [&](size_t i, size_t j) -> int& { return dp[i * stride + j]; };
    for (size_t i = 1; i <= n; ++i)
        for (size_t j = 1; j <= m; ++j)
            at(i, j) = a[i - 1] == b[j - 1] ? at(i - 1, j - 1) + 1
                                           : max(at(i - 1, j), at(i, j - 1));
    string result;
    result.reserve(at(n, m));
    while (n && m) {
        if (a[n - 1] == b[m - 1]) {
            result.push_back(a[--n]);
            --m;
        } else if (at(n - 1, m) >= at(n, m - 1)) {
            --n;
        } else {
            --m;
        }
    }
    reverse(result.begin(), result.end());
    return result;
}
}
