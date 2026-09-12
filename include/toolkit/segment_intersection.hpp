#pragma once
#include "geometry_point.hpp"

namespace toolkit {
// Repository-original exact closed-segment predicate. No upstream code copied.
// |coordinate|<=1e18 is checked; differences/products are formed in signed
// __int128 (determinants <=8e36). Includes points, endpoints and collinear overlap.
inline bool segments_intersect(IntegerPoint a, IntegerPoint b, IntegerPoint c, IntegerPoint d) {
    for (IntegerPoint p : {a, b, c, d}) check_geometry_point(p);
    auto turn = [](IntegerPoint p, IntegerPoint q, IntegerPoint r) {
        __int128 cross = (__int128(q.x) - p.x) * (__int128(r.y) - p.y)
                       - (__int128(q.y) - p.y) * (__int128(r.x) - p.x);
        return (cross > 0) - (cross < 0);
    };
    auto overlap = [](long long a0, long long a1, long long b0, long long b1) {
        return max(min(a0, a1), min(b0, b1)) <= min(max(a0, a1), max(b0, b1));
    };
    return overlap(a.x, b.x, c.x, d.x) && overlap(a.y, b.y, c.y, d.y)
        && turn(a, b, c) * turn(a, b, d) <= 0
        && turn(c, d, a) * turn(c, d, b) <= 0;
}
}
