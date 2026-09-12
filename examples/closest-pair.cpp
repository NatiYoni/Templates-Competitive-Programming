#include "toolkit/closest_pair.hpp"

// Unchanged KACTL CC0 ordered-set sweep; O(n log n) time/O(n) space.
// Checked integer |coordinate|<=1e8; squared distances <=8e16 fit int64.
// Duplicate entries can be the closest pair (zero distance); <2 gives nullopt.
// Returns positions, not original indices; ties need not select a unique pair.
// Input {(0,0),(8,9),(1,1)}. Output: 2.
int main() {
    auto result = toolkit::closest_pair({{0, 0}, {8, 9}, {1, 1}});
    if (result) cout << result->squared_distance << '\n';
}
