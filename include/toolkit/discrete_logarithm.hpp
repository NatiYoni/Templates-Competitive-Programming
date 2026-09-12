#pragma once
#include "modular_u64.hpp"

// Independently authored gcd-stripping + ordered-map baby-step giant-step.
namespace toolkit {
// Smallest x>=0 with a^x == b (mod m), including non-coprime a,m.
// 1<=m<=UINT32_MAX; a,b may be any uint64_t and are reduced. Modulus 1
// returns 0; 0^0=1, so for m>1: log_0(1)=0 and log_0(0)=1.
// nullopt means no solution, not work exhaustion. Larger moduli are rejected.
inline optional<uint64_t> discrete_log(uint64_t a, uint64_t b, uint64_t m) {
    if (!m || m > UINT32_MAX) throw invalid_argument("discrete log requires 1<=m<=UINT32_MAX");
    a %= m;
    b %= m;
    uint64_t removed = 0, multiplier = 1 % m;
    while (true) {
        if (b == multiplier) return removed;
        uint64_t common = gcd(a, m);
        if (common == 1) break;
        if (b % common) return nullopt;
        b /= common;
        m /= common;
        multiplier = multiply_mod_u64(multiplier, a / common, m);
        ++removed;
    }

    uint64_t width = uint64_t(sqrt(static_cast<long double>(m)));
    while (width * width < m) ++width;
    while (width && (width - 1) * (width - 1) >= m) --width;
    map<uint64_t, uint64_t> babies;
    uint64_t value = b;
    for (uint64_t j = 0; j < width; ++j) {
        babies[value] = j; // Largest j gives the smallest exponent in this block.
        value = multiply_mod_u64(value, a, m);
    }
    uint64_t stride = power_mod_u64(a, width, m), giant = multiplier;
    for (uint64_t i = 1; i <= width + 1; ++i) {
        giant = multiply_mod_u64(giant, stride, m);
        auto found = babies.find(giant);
        if (found != babies.end()) return removed + i * width - found->second;
    }
    return nullopt;
}
}
