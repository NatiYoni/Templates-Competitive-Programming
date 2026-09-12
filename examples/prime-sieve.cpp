#include "toolkit/kactl_prelude.hpp"
#include "content/number-theory/Eratosthenes.h"

// KACTL eratosthenesSieve(limit) returns primes strictly LESS than limit.
// 0<=limit<=MAX_PR=5,000,000; only isprime[0..limit) is meaningful afterward.
// Every call resets the global bitset (including invalid answers outside limit);
// calls are not thread-safe. O(MAX_PR/word_size + limit log log limit) time.
// Fixed MAX_PR-bit storage plus O(number of primes) output; no recursion.
// Input limit=20; output: 2 3 5 7 11 13 17 19
int main() {
    auto primes = eratosthenesSieve(20);
    for (int i = 0; i < sz(primes); ++i) cout << (i ? " " : "") << primes[i];
    cout << '\n';
}
