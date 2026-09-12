#include "toolkit/rolling_hash.hpp"

// Repository-original polynomial fingerprint over byte+1 in the prime field 2^61-1.
// Pass ONE immutable shared context to all comparable strings; seed is explicit.
// O(n) construction/storage, O(1) half-open slices/comparisons, no mutable globals.
// Matching length/hash is only a candidate: randomized collisions remain possible,
// fixed known seeds offer no adversarial protection, and this is not cryptography.
// Input banana/ananas: equal substrings "ana"; output: 1 1 (filter, then exact).
int main() {
    auto context = make_shared<const toolkit::RollingHashContext>(1729);
    string a = "banana", b = "ananas";
    toolkit::RollingHash ha(a, context), hb(b, context);
    bool candidate = ha.possibly_equal(1, 4, hb, 0, 3);
    bool exact = candidate && a.compare(1, 3, b, 0, 3) == 0;
    cout << candidate << ' ' << exact << '\n';
}
