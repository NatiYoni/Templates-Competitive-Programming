#pragma once
#include "base.hpp"

// Independently authored toolkit arithmetic; no copied upstream implementation.
namespace toolkit {
inline uint64_t multiply_mod_u64(uint64_t a, uint64_t b, uint64_t modulus) {
    if (!modulus) throw invalid_argument("zero modulus");
    return __uint128_t(a) * b % modulus;
}

// 0^0 is 1 before reduction; modulus 1 therefore always returns 0.
inline uint64_t power_mod_u64(uint64_t a, uint64_t exponent, uint64_t modulus) {
    if (!modulus) throw invalid_argument("zero modulus");
    uint64_t result = 1 % modulus;
    while (exponent) {
        if (exponent & 1) result = multiply_mod_u64(result, a, modulus);
        exponent >>= 1;
        if (exponent) a = multiply_mod_u64(a, a, modulus);
    }
    return result;
}
}
