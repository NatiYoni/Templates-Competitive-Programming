#include "toolkit/base.hpp"
#include <atcoder/convolution.hpp>
#include <atcoder/modint.hpp>

// ACL convolution multiplies coefficient vectors in ascending degree order.
// This usage fixes prime 998244353: nonempty inputs need n+m-1<=2^23.
// More generally the next power of two must divide the chosen prime minus one.
// Empty operand -> {}; inputs unchanged for lvalues. O(L log L) time/O(L) space
// (quadratic fallback when min(n,m)<=60); static root tables initialize lazily.
// Input (1+2x+3x^2)*(4+5x); output: 4 13 22 15
int main() {
    using Mint = atcoder::modint998244353;
    vector<Mint> a{1, 2, 3}, b{4, 5};
    auto product = atcoder::convolution(a, b);
    for (int i = 0; i < int(product.size()); ++i) cout << (i ? " " : "") << product[i].val();
    cout << '\n';
}
