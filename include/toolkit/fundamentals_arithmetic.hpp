#pragma once
#include "base.hpp"

namespace toolkit {
namespace fundamentals_detail {
inline long long checked_add(long long a, long long b) {
    if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b))
        throw overflow_error("signed sum");
    return a + b;
}

inline long long checked_subtract(long long a, long long b) {
    if ((b > 0 && a < LLONG_MIN + b) || (b < 0 && a > LLONG_MAX + b))
        throw overflow_error("signed difference");
    return a - b;
}
}
}
