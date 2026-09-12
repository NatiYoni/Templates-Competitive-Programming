#include "toolkit/base.hpp"
#include <atcoder/math.hpp>

// ACL crt(residues,moduli) solves x==r[i] (mod m[i]); equal vector sizes,
// m[i]>0, signed residues allowed, total LCM must fit signed long long.
// Returns (least nonnegative residue,LCM), (0,0) if inconsistent, (0,1) if empty.
// O(k log(max modulus)) time, O(1) auxiliary space, no mutation/global state.
// Input residues 2,8 moduli 6,9; output: 8 18
int main() {
    auto [residue, period] = atcoder::crt({2, 8}, {6, 9});
    cout << residue << ' ' << period << '\n';
}
