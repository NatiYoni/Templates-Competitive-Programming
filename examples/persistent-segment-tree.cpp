#include "toolkit/persistent_segment_tree.hpp"

// Fixed: assign 5 at position 1, branch twice. Expected: 0 5 12 -2
int main() {
    toolkit::PersistentRangeSum tree(4);
    size_t a = tree.set(0, 1, 5), b = tree.set(a, 3, 7), c = tree.set(0, 1, -2);
    for (size_t v : {size_t(0), a, b, c})
        cout << (v ? " " : "") << (long long)tree.sum(v, 0, 4);
    cout << '\n'; // Fixed small sums fit long long; the API returns __int128.
}
