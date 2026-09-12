#include "toolkit/base.hpp"
#include <atcoder/modint.hpp>

// ACL static/dynamic modints normalize signed inputs to [0,mod).
// Positive modulus; dynamic documented bound <=2,000,001,000. Division/inv
// requires gcd(value,mod)=1; pow requires exponent>=0; raw needs 0<=value<mod.
// Set a dynamic ID's modulus BEFORE constructing values; discard all those
// values before changing it. Each ID shares global state, not thread-safe.
// Arithmetic O(1), power O(log exponent), inverse O(log mod), O(1) object space.
// Output: 2 9
//         11 5
int main() {
    using Static = atcoder::static_modint<17>;
    Static a = -3, b = 5;
    cout << (a*b).val() << ' ' << Static(2).inv().val() << '\n';
    using Dynamic = atcoder::dynamic_modint<0>;
    Dynamic::set_mod(12);
    Dynamic x = -1, y = 5;
    cout << x.val() << ' ' << y.inv().val() << '\n';
}
