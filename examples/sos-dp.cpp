#include "toolkit/subset_transforms.hpp"

// Two-bit mask table [1,2,3,4]. Subset sums: 1 3 4 10
int main() {
    auto result = toolkit::subset_zeta_sum({1, 2, 3, 4});
    for (size_t i = 0; i < result.size(); ++i) cout << (i ? " " : "") << result[i];
    cout << '\n';
}
