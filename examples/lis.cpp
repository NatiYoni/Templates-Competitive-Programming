#include "toolkit/kactl_prelude.hpp"
#include "content/various/LIS.h"

// KACTL lis(values) returns zero-based INDICES, strictly increasing in both index
// and value. Duplicate values do not extend the LIS. Empty input -> {}.
// O(n log n) time, O(n) space, no mutation; tie reconstruction is not canonical.
// Input 3,1,2,2,4; output indices, then values:
// 1 3 4
// 1 2 4
int main() {
    vi values{3, 1, 2, 2, 4};
    auto indices = lis(values);
    for (int i = 0; i < sz(indices); ++i) cout << (i ? " " : "") << indices[i];
    cout << '\n';
    for (int i = 0; i < sz(indices); ++i) cout << (i ? " " : "") << values[indices[i]];
    cout << '\n';
}
