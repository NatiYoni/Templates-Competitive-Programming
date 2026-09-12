#include "toolkit/kactl_prelude.hpp"
#include "content/geometry/ConvexHull.h"

// KACTL convexHull copies integer Point<long long> input and returns the CCW
// extreme vertices, starting lexicographically smallest, without repeating start.
// Empty -> {}; all duplicates -> one point; collinear -> the two endpoints.
// Collinear edge-interior points are omitted. All differences/cross products
// must fit int64; |coordinate|<=1e8 is sufficient here. O(n log n) time/O(n)
// storage; local sorting has O(log n) library stack, no algorithm recursion.
// Input square plus duplicate/interior point; output: (0,0) (2,0) (2,2) (0,2)
int main() {
    vector<P> points{P(0, 0), P(2, 0), P(2, 2), P(0, 2), P(1, 1), P(0, 0)};
    auto hull = convexHull(points);
    for (int i = 0; i < sz(hull); ++i) cout << (i ? " " : "") << hull[i];
    cout << '\n';
}
