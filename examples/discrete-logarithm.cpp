#include "toolkit/discrete_logarithm.hpp"

int main() {
    for (auto input : {array<uint64_t, 3>{4, 8, 14}, {2, 0, 8}, {2, 3, 8}, {0, 1, 7}}) {
        auto exponent = toolkit::discrete_log(input[0], input[1], input[2]);
        cout << input[0] << "^x = " << input[1] << " mod " << input[2] << ": ";
        if (exponent) cout << *exponent << '\n'; // 3, 3, no solution, 0.
        else cout << "no solution\n";
    }
}
