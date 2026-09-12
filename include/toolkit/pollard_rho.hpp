#pragma once
#include "miller_rabin.hpp"

// Independently authored, bounded-retry Floyd Pollard rho. A work cap bounds
// attempts, NOT the runtime required to successfully factor every input.
namespace toolkit {
struct RhoBudget {
    size_t attempts_per_split = 32;
    size_t steps_per_attempt = 250000;
};

namespace rho_detail {
inline optional<uint64_t> split(uint64_t n, mt19937_64& random, RhoBudget budget) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    for (size_t attempt = 0; attempt < budget.attempts_per_split; ++attempt) {
        uint64_t x = 2 + random() % (n - 2), y = x;
        uint64_t c = 1 + random() % (n - 1);
        auto step = [=](uint64_t v) {
            return uint64_t((__uint128_t(v) * v + c) % n);
        };
        for (size_t i = 0; i < budget.steps_per_attempt; ++i) {
            x = step(x);
            y = step(step(y));
            uint64_t divisor = gcd(x > y ? x - y : y - x, n);
            if (divisor == n) break;
            if (divisor > 1) return divisor;
        }
    }
    return nullopt;
}
}

// n>=1. A seed resets a local PRNG, so retries and failures are reproducible.
// Success is the complete sorted prime multiset (empty for 1). nullopt discards
// ALL partial factors. Call again with another seed/budget to retry explicitly.
inline optional<vector<uint64_t>> factor_u64(uint64_t n, uint64_t seed,
                                             RhoBudget budget = {}) {
    if (!n) throw invalid_argument("factorization of zero");
    mt19937_64 random(seed);
    vector<uint64_t> pending{n}, result;
    while (!pending.empty()) {
        uint64_t value = pending.back();
        pending.pop_back();
        if (value == 1) continue;
        if (is_prime_u64(value)) { result.push_back(value); continue; }
        auto divisor = rho_detail::split(value, random, budget);
        if (!divisor) return nullopt;
        pending.push_back(*divisor);
        pending.push_back(value / *divisor);
    }
    sort(result.begin(), result.end());
    return result;
}
}
