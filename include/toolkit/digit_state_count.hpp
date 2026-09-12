#pragma once
#include "base.hpp"

namespace toolkit {
// Worked decimal digit-state DP, NOT a universal digit-DP solver.
// Count x in inclusive unsigned-64 [lo,hi] whose canonical decimal digits have
// no equal neighbors and sum % modulus == residue. Leading padding zeroes do
// not enter the state. The number zero has digit sum 0, no repeated neighbors,
// and is counted exactly once iff include_zero and residue==0.
// modulus in [1,200], residue in [0,modulus), lo<=hi; otherwise throws.
// Tight prefixes never exceed the bound. Unsigned __int128 counts include the
// possible 2^64-sized input interval without wrapping. No globals/recursion.
// O(20*modulus*11*10) time, O(modulus*11) memory, input unchanged.
inline unsigned __int128 count_no_adjacent_digit_sum(
    unsigned long long lo, unsigned long long hi, int modulus, int residue,
    bool include_zero = true) {
    if (lo > hi || modulus < 1 || modulus > 200 || residue < 0 || residue >= modulus)
        throw invalid_argument("digit-state bounds/modulus/residue");
    auto prefix = [&](unsigned long long bound) {
        using Count = unsigned __int128;
        auto index = [](int sum, int previous, int tight) {
            return (sum * 11 + previous) * 2 + tight;
        };
        vector<Count> dp(size_t(modulus) * 22), next(dp.size());
        dp[index(0, 10, 1)] = 1; // previous=10 denotes an unstarted number.
        for (char c : to_string(bound)) {
            fill(next.begin(), next.end(), 0);
            int digit_bound = c - '0';
            for (int s = 0; s < modulus; ++s)
                for (int p = 0; p <= 10; ++p)
                    for (int tight = 0; tight <= 1; ++tight) {
                        Count ways = dp[index(s, p, tight)];
                        if (!ways) continue;
                        for (int d = 0; d <= (tight ? digit_bound : 9); ++d) {
                            if (p != 10 && p == d) continue;
                            int next_p = p == 10 && d == 0 ? 10 : d;
                            int next_s = next_p == 10 ? 0 : (s + d) % modulus;
                            next[index(next_s, next_p, tight && d == digit_bound)] += ways;
                        }
                    }
            dp.swap(next);
        }
        Count total = 0;
        for (int p = 0; p <= 10; ++p)
            if (p != 10 || include_zero)
                total += dp[index(residue, p, 0)] + dp[index(residue, p, 1)];
        return total;
    };
    return prefix(hi) - (lo ? prefix(lo - 1) : 0);
}
}
