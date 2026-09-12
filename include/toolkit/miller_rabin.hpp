#pragma once
#include "modular_u64.hpp"

// Independently authored Miller-Rabin control flow, using the standard seven
// deterministic witnesses for n < 2^64, not a floating-point modmul variant.
namespace toolkit {
inline bool is_prime_u64(uint64_t n) {
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    uint64_t odd = n - 1;
    unsigned shifts = 0;
    while ((odd & 1) == 0) { odd >>= 1; ++shifts; }
    for (uint64_t witness : {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL,
                             9780504ULL, 1795265022ULL}) {
        if (witness % n == 0) continue;
        uint64_t value = power_mod_u64(witness, odd, n);
        if (value == 1 || value == n - 1) continue;
        bool reaches_minus_one = false;
        for (unsigned i = 1; i < shifts; ++i) {
            value = multiply_mod_u64(value, value, n);
            if (value == n - 1) { reaches_minus_one = true; break; }
        }
        if (!reaches_minus_one) return false;
    }
    return true;
}
}
