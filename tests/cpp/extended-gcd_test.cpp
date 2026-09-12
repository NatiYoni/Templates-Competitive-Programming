#include "toolkit/extended_gcd.hpp"
#include "test_util.hpp"

uint64_t random64() { return (uint64_t(rng()) << 32) | rng(); }
uint64_t magnitude(int64_t n) { return n < 0 ? uint64_t(-__int128_t(n)) : uint64_t(n); }
uint64_t binary_gcd(uint64_t a, uint64_t b) {
    if (!a || !b) return a | b;
    unsigned common = 0;
    while (((a | b) & 1) == 0) { a >>= 1; b >>= 1; ++common; }
    while ((a & 1) == 0) a >>= 1;
    while (b) {
        while ((b & 1) == 0) b >>= 1;
        if (a > b) swap(a, b);
        b -= a;
    }
    return a << common;
}
void check(int64_t a, int64_t b) {
    ++cases_checked;
    string input = "a=" + to_string(a) + " b=" + to_string(b);
    auto value = toolkit::extended_gcd(a, b);
    require(value.gcd == binary_gcd(magnitude(a), magnitude(b)), input + " gcd");
    require(__int128_t(a) * value.x + __int128_t(b) * value.y == value.gcd, input + " Bezout");
    __int128_t bound = __int128_t(1) << 63;
    require(-bound <= value.x && value.x <= bound && -bound <= value.y && value.y <= bound,
            input + " coefficient bounds");
    if (!a && !b) require(!value.x && !value.y, input + " zero convention");
}

int main() {
    for (int64_t a = -32; a <= 32; ++a)
        for (int64_t b = -32; b <= 32; ++b) check(a, b);
    vector<int64_t> edges{INT64_MIN, INT64_MIN + 1, -1000000007, -1, 0, 1,
                           2, 1000000007, INT64_MAX - 1, INT64_MAX};
    for (auto a : edges) for (auto b : edges) check(a, b);
    for (int iteration = 0; iteration < 1500; ++iteration) {
        int64_t a = int64_t(random64() >> 1), b = int64_t(random64() >> 1);
        if (rng() & 1) a = -a;
        if (rng() & 1) b = -b;
        check(a, b);
    }
    int64_t previous = 1, current = 1;
    while (previous <= INT64_MAX - current) {
        check(previous, current);
        int64_t next = previous + current;
        previous = current;
        current = next;
    }
    success();
}
