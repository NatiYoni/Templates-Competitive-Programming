#pragma once
#include "base.hpp"
#include "math/number-theory/extgcd.hpp"

// Authored sign/domain wrapper around unchanged Luzhiled extgcd (Unlicense).
namespace toolkit {
struct Bezout {
    uint64_t gcd;
    __int128_t x, y;
};

// Full int64_t inputs, including INT64_MIN. Positive gcd may equal 2^63.
// Widen BEFORE negation, and instantiate the upstream recurrence in int128.
// Coefficients are not normalized inverses. (0,0) returns {0,0,0}.
inline Bezout extended_gcd(int64_t a, int64_t b) {
    if (!a && !b) return {0, 0, 0};
    __int128_t aa = a, bb = b, x, y;
    if (aa < 0) aa = -aa;
    if (bb < 0) bb = -bb;
    __int128_t divisor = ::extgcd(aa, bb, x, y);
    return {uint64_t(divisor), a < 0 ? -x : x, b < 0 ? -y : y};
}
}
