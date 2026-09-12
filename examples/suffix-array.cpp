#include "toolkit/base.hpp"
#include <atcoder/string.hpp>

// ACL suffix_array: zero-based suffix starts, lexicographic order; empty -> {}.
// This string usage is restricted to ASCII [0,127]: upstream converts signed char
// directly to int. For arbitrary bytes use nonnegative vector<int> and upper=255.
// Bounded alphabet overload needs 0<=s[i]<=upper, O(n+upper) time/space.
// Generic vector overload sorts/compresses in O(n log n); no input mutation.
// Input banana; output: 5 3 1 0 4 2
int main() {
    auto sa = atcoder::suffix_array(string("banana"));
    for (int i = 0; i < int(sa.size()); ++i) cout << (i ? " " : "") << sa[i];
    cout << '\n';
}
