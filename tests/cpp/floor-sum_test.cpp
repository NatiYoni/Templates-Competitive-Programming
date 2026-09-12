#include <atcoder/math.hpp>
#include "test_util.hpp"

void check(long long n, long long m, long long a, long long b) {
    ++cases_checked;
    __int128 expected = 0;
    for (long long i = 0; i < n; ++i) {
        __int128 numerator = __int128(a) * i + b;
        __int128 quotient = numerator / m;
        if (numerator % m < 0) --quotient;
        expected += quotient;
    }
    string input = "n=" + to_string(n) + " m=" + to_string(m) +
                   " a=" + to_string(a) + " b=" + to_string(b);
    require(LLONG_MIN <= expected && expected <= LLONG_MAX, input + " oracle range");
    require(atcoder::floor_sum(n, m, a, b) == static_cast<long long>(expected), input);
}

int main() {
    check(0, 1, LLONG_MIN, LLONG_MAX);
    check(1, 1, LLONG_MIN, LLONG_MIN);
    check(2, (1LL << 32) - 1, LLONG_MIN, LLONG_MAX);
    check(9, 7, -3, -11);
    check(9, 1, -3, -11);
    for (int t = 0; t < 400; ++t)
        check(rng() % 60, 1 + rng() % 60, int(rng() % 601) - 300, int(rng() % 601) - 300);
    const long long n = (1LL << 32) - 1, triangular = n * ((n - 1) / 2);
    ++cases_checked;
    require(atcoder::floor_sum(n, n, n, 0) == triangular,
            "n=m=a=4294967295 b=0 maximum n and m");
    ++cases_checked;
    require(atcoder::floor_sum(n, n, -n, 0) == -triangular,
            "n=m=4294967295 a=-4294967295 b=0");
    ++cases_checked;
    require(atcoder::floor_sum(n, n, 0, n - 1) == 0,
            "n=m=4294967295 a=0 b=4294967294");
    success();
}
