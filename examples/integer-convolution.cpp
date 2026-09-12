#include "toolkit/base.hpp"
#include <atcoder/convolution.hpp>

// ACL convolution_ll returns exact signed long long polynomial coefficients.
// Empty operand -> {}; otherwise n+m-1<=2^24 and EVERY result coefficient must
// fit long long. A sufficient bound is min(n,m)*max|a|*max|b|<=LLONG_MAX
// (check it in a wider type). Negative coefficients are supported, not rounded.
// Three modular transforms plus CRT: O(L log L) time, O(L) space; small sizes
// use quadratic products. Lvalue inputs unchanged; static root tables, no recursion.
// Input (-1+2x-3x^2)*(4-5x); output: -4 13 -22 15
int main() {
    vector<long long> a{-1, 2, -3}, b{4, -5};
    auto product = atcoder::convolution_ll(a, b);
    for (int i = 0; i < int(product.size()); ++i) cout << (i ? " " : "") << product[i];
    cout << '\n';
}
