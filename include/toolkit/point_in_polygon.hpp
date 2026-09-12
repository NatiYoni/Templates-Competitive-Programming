#pragma once
#include "geometry_point.hpp"
#include "kactl_prelude.hpp"
#include "content/geometry/InsidePolygon.h"

namespace toolkit {
enum class PolygonLocation { outside, boundary, inside };

// Independently authored checked adapter over unchanged KACTL CC0 predicates.
// Conversion BEFORE subtraction/products gives exact signed-128-bit arithmetic.
// Empty -> outside; one/two vertices have only boundary and outside locations.
inline PolygonLocation point_in_polygon(const vector<IntegerPoint>& polygon, IntegerPoint query) {
    if (polygon.size() >= size_t(INT_MAX)) throw length_error("polygon vertex bound");
    check_geometry_point(query);
    vector<Point<__int128>> wide;
    wide.reserve(polygon.size());
    for (auto p : polygon) {
        check_geometry_point(p);
        wide.emplace_back(p.x, p.y);
    }
    Point<__int128> q(query.x, query.y);
    if (inPolygon(wide, q, true)) return PolygonLocation::inside;
    return inPolygon(wide, q, false) ? PolygonLocation::boundary : PolygonLocation::outside;
}
}
