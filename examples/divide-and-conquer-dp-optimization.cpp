#include "toolkit/partition_dp.hpp"

// Nonnegative weights imply Monge squared segment sums, hence monotone optima.
// Exactly two nonempty groups of [1,2,3,4]: [1,2,3]|[4]. Expected: 52
int main() {
    vector<long long> prefix{0, 1, 3, 6, 10};
    auto dp = toolkit::partition_dp(4, 2, [&](int l, int r) {
        long long sum = prefix[r] - prefix[l];
        return sum * sum; // Fixed small sums; callers must bound their products.
    });
    cout << *dp[4] << '\n';
}
