#include "toolkit/segment_intersection.hpp"

// Repository-original exact closed-segment intersection predicate; O(1).
// All coordinates in [-1e18,1e18], checked before signed __int128 arithmetic.
// Endpoints, collinear overlap and zero-length segments count as intersections.
// No intersection coordinates are returned; there is no floating-point epsilon.
// Output: 1 1 0 (proper crossing, endpoint/point, separated collinear).
int main() {
    cout << toolkit::segments_intersect({0, 0}, {3, 3}, {0, 3}, {3, 0}) << ' '
         << toolkit::segments_intersect({2, 0}, {2, 0}, {0, 0}, {2, 0}) << ' '
         << toolkit::segments_intersect({0, 0}, {1, 0}, {2, 0}, {3, 0}) << '\n';
}
