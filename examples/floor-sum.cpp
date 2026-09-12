#include "toolkit/base.hpp"
#include <atcoder/math.hpp>

// ACL floor_sum(n,m,a,b) = sum over 0<=i<n of floor((a*i+b)/m).
// 0<=n<2^32, 1<=m<2^32; a,b are signed long long and negatives are normalized.
// Use a signed-64-bit-representable final sum for an exact signed answer; upstream
// accumulates modulo 2^64 on overflow. Do not form overflowing a*i in caller code.
// O(log m) time, O(1) space; no mutation, global state, or recursion.
// Input n=4,m=3,a=-2,b=5: terms 1,1,0,-1; output: 1
int main() {
    cout << atcoder::floor_sum(4, 3, -2, 5) << '\n';
}
