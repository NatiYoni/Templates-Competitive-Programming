#include "toolkit/kactl_prelude.hpp"
#include "content/geometry/PolygonArea.h"

// KACTL polygonArea2 takes a NONEMPTY ordered polygon and returns twice signed
// area (CCW positive). Singleton/two points/collinear vertices give zero.
// No input mutation despite non-const reference; O(n) time/O(1) extra space.
// Use int64 with all cross products and partial sums in range. A sufficient bound
// for |coordinate|<=B is 2*n*B^2<=LLONG_MAX. Self-crossing input gives algebraic,
// not union, area. For ordinary geometric area require a simple boundary order.
// Input CCW 3-by-2 rectangle; output (twice signed area, area): 12 6
int main() {
    vector<Point<long long>> polygon{Point<ll>(0, 0), Point<ll>(3, 0),
                                     Point<ll>(3, 2), Point<ll>(0, 2)};
    long long twice_signed_area = polygonArea2(polygon);
    cout << twice_signed_area << ' ' << abs(twice_signed_area) / 2.0 << '\n';
}
