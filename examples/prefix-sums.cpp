#include "toolkit/prefix_sums.hpp"

// Input (fixed): [2,-3,5,4], ranges [1,4), [2,2). Expected output: 6 0
int main() {
    toolkit::PrefixSums sums({2, -3, 5, 4});
    cout << sums.range(1, 4) << ' ' << sums.range(2, 2) << '\n';
}
