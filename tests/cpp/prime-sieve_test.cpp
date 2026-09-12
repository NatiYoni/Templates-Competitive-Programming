#include "toolkit/kactl_prelude.hpp"
#include "content/number-theory/Eratosthenes.h"
#include "test_util.hpp"

bool trial_prime(int x) {
    if (x < 2) return false;
    for (int d = 2; d <= x / d; ++d) if (x % d == 0) return false;
    return true;
}

void check(int limit) {
    ++cases_checked;
    vi expected;
    for (int x = 0; x < limit; ++x) if (trial_prime(x)) expected.push_back(x);
    auto actual = eratosthenesSieve(limit);
    require(actual == expected, "exclusive limit=" + to_string(limit));
    for (int x = 0; x < limit; ++x)
        require(isprime[x] == trial_prime(x), "limit=" + to_string(limit) + " x=" + to_string(x));
}

int main() {
    for (int limit : {0, 1, 2, 3, 4, 5, 6, 10, 49, 50, 121, 122}) check(limit);
    for (int t = 0; t < 300; ++t) check(int(rng() % 700));
    ++cases_checked;
    // Independent linear least-prime-factor sieve checks the entire fixed-capacity boundary.
    vector<int> least(MAX_PR);
    vi expected;
    for (int x = 2; x < MAX_PR; ++x) {
        if (!least[x]) { least[x] = x; expected.push_back(x); }
        for (int p : expected) {
            if (p > least[x] || 1LL * x * p >= MAX_PR) break;
            least[x * p] = p;
        }
    }
    auto actual = eratosthenesSieve(MAX_PR);
    require(actual == expected, "limit=MAX_PR=5000000 linear-sieve oracle");
    for (int x = MAX_PR - 100; x < MAX_PR; ++x)
        require(isprime[x] == trial_prime(x), "limit=MAX_PR x=" + to_string(x));
    check(4);
    ++cases_checked;
    // set() resets the whole bitset; entries outside the last limit are not prime answers.
    require(isprime[9], "after limit=4, bit 9 resets to true and is out of contract");
    check(100);
    check(0);
    success();
}
