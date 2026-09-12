#include "toolkit/xor_linear_basis.hpp"

int main() {
    toolkit::XorBasis basis;
    for (uint64_t value : {3, 5, 6}) basis.insert(value);
    cout << basis.rank() << ' ' << basis.contains(6) << ' '
         << basis.maximum_xor() << '\n'; // 2 1 6; not a subset-sum basis.
    basis.insert(uint64_t(1) << 63);
    cout << basis.maximum_xor(1) << '\n';
}
