#include "toolkit/knuth_merge_dp.hpp"

// Nonnegative adjacent merges [3,1,2], optimized with Knuth bounds. Expected: 9
int main() {
    cout << (long long)toolkit::knuth_adjacent_merge_cost({3, 1, 2}) << '\n';
}
