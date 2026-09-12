#pragma once
#include "geometry_point.hpp"
#include "kactl_prelude.hpp"
#include "content/geometry/ClosestPair.h"

namespace toolkit {
struct ClosestPair {
    IntegerPoint first, second;
    long long squared_distance;
};

// Independently authored checked optional/result adapter; KACTL CC0 sweep unchanged.
// Bound 1e8 makes every upstream squared distance <=8e16, below LLONG_MAX.
// Empty/singleton -> nullopt; duplicate positions from different entries give zero.
inline optional<ClosestPair> closest_pair(const vector<IntegerPoint>& points) {
    if (points.size() >= size_t(INT_MAX)) throw length_error("closest pair size");
    vector<Point<long long>> input;
    input.reserve(points.size());
    for (auto p : points) {
        check_geometry_point(p, 100000000);
        input.emplace_back(p.x, p.y);
    }
    if (points.size() < 2) return nullopt;
    auto [a, b] = closest(std::move(input));
    return ClosestPair{{a.x, a.y}, {b.x, b.y}, (a - b).dist2()};
}
}
