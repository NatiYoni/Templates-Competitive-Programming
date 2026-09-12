#include "toolkit/interval_merge_dp.hpp"

// Worked adjacent merges [3,1,2]: merge 1+2 for 3, then 3+3 for 6. Expected: 9
int main() {
    cout << (long long)toolkit::adjacent_merge_cost({3, 1, 2}) << '\n';
}
