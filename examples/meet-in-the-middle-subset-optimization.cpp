#include "toolkit/meet_in_middle_subset.hpp"

// Signed subset optimization: [8,-3,4], bound 6. Best is 8-3=5, mask 3.
// Expected: 5 3
int main() {
    auto best = toolkit::meet_in_middle_subset({8, -3, 4}, 6);
    if (best) cout << (long long)best->sum << ' ' << best->mask << '\n';
    else cout << "infeasible\n";
}
