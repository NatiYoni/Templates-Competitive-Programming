#include "toolkit/kactl_prelude.hpp"
#include "content/data-structures/RMQ.h"

// KACTL RMQ (Johan Sannemo, pajenegod; CC0; Source: Folklore).
// Immutable copied array; query [l,r) requires 0 <= l < r <= n.
// An empty array may be constructed, but never queried. Build/space O(n log n),
// queries O(1); GNU __builtin_clz and int indices (use n <= 1e8).
// Input [8,-2,5,-2,9], queries [1,4), [4,5). Output: -2 9
int main() {
    RMQ<int> minima(vector<int>{8, -2, 5, -2, 9});
    cout << minima.query(1, 4) << ' ' << minima.query(4, 5) << '\n';
}
