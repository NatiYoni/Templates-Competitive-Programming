#pragma once
#include "base.hpp"

namespace toolkit {
// Repository-original boundary types/validation, not copied from an upstream template.
struct IntegerPoint {
    long long x, y;
    bool operator==(IntegerPoint other) const { return x == other.x && y == other.y; }
    bool operator<(IntegerPoint other) const { return tie(x, y) < tie(other.x, other.y); }
    friend ostream& operator<<(ostream& out, IntegerPoint p) {
        return out << '(' << p.x << ',' << p.y << ')';
    }
};
inline void check_geometry_point(IntegerPoint p, long long bound = 1000000000000000000LL) {
    if (p.x < -bound || p.x > bound || p.y < -bound || p.y > bound)
        throw out_of_range("geometry coordinate bound");
}
}
