#include "toolkit/base.hpp"
#include <atcoder/string.hpp>

// ACL z_algorithm(s)[i] is the prefix/suffix-at-i LCP; z[0]=|s|, empty -> {}.
// String or equality-comparable vector input; int-sized lengths, O(n) time/space.
// No input mutation or global state. Input ABABABA; output: 7 0 5 0 3 0 1
int main() {
    auto z = atcoder::z_algorithm(string("ABABABA"));
    for (int i = 0; i < int(z.size()); ++i) cout << (i ? " " : "") << z[i];
    cout << '\n';
}
