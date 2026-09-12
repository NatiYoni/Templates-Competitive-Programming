#include "toolkit/segment_intersection.hpp"
#include "test_util.hpp"

using Q = toolkit::IntegerPoint;
using Wide = __int128;

// Independent oracle: solve the two line parameters as exact rational numbers,
// then clip them to [0,1]. Parallel lines use one-dimensional interval clipping.
bool parametric(Q a, Q b, Q c, Q d) {
    Wide rx = Wide(b.x) - a.x, ry = Wide(b.y) - a.y;
    Wide sx = Wide(d.x) - c.x, sy = Wide(d.y) - c.y;
    Wide dx = Wide(c.x) - a.x, dy = Wide(c.y) - a.y;
    Wide determinant = rx * sy - ry * sx;
    if (determinant) {
        Wide t = dx * sy - dy * sx, u = dx * ry - dy * rx;
        if (determinant < 0) determinant = -determinant, t = -t, u = -u;
        return 0 <= t && t <= determinant && 0 <= u && u <= determinant;
    }
    if (a == b) {
        if (c == d) return a == c;
        Wide px = Wide(a.x) - c.x, py = Wide(a.y) - c.y;
        if (px * sy != py * sx) return false;
        Wide t = sx ? px : py, denominator = sx ? sx : sy;
        if (denominator < 0) denominator = -denominator, t = -t;
        return 0 <= t && t <= denominator;
    }
    if (dx * ry != dy * rx) return false;
    Wide length = rx ? rx : ry, lo = rx ? dx : dy;
    Wide hi = lo + (rx ? sx : sy);
    if (length < 0) length = -length, lo = -lo, hi = -hi;
    if (lo > hi) swap(lo, hi);
    return lo <= length && hi >= 0;
}
void check(Q a, Q b, Q c, Q d) {
    ++cases_checked;
    string input = "segments=" + show(vector<Q>{a, b, c, d});
    bool expected = parametric(a, b, c, d);
    require(toolkit::segments_intersect(a, b, c, d) == expected, input + " exact line parameters");
    require(toolkit::segments_intersect(b, a, d, c) == expected, input + " reversed endpoints");
    require(toolkit::segments_intersect(c, d, a, b) == expected, input + " swapped segments");
}
int main() {
    vector<Q> grid;
    for (long long x = -1; x <= 1; ++x)
        for (long long y = -1; y <= 1; ++y) grid.push_back({x, y});
    for (Q a : grid) for (Q b : grid) for (Q c : grid) for (Q d : grid) check(a, b, c, d);
    const long long big = 1000000000000000000LL;
    check({-big, -big}, {big, big}, {-big, big}, {big, -big});
    check({-big, 0}, {big, 0}, {big, 0}, {big, big});
    check({-big, -big}, {-big, big}, {big, -big}, {big, big});
    check({big, big}, {big, big}, {big, big}, {big, big});
    check({0, 0}, {3, 3}, {0, 3}, {3, 0}); // nonintegral intersection
    for (int t = 0; t < 450; ++t) {
        vector<Q> p(4);
        long long scale = t % 3 ? 1 : 10000000000000000LL;
        for (Q& q : p) q = {(int(rng() % 201) - 100) * scale,
                            (int(rng() % 201) - 100) * scale};
        if (t % 5 == 0) p[1] = p[0];
        if (t % 7 == 0) p[3] = p[1];
        check(p[0], p[1], p[2], p[3]);
    }
    for (long long bad : {LLONG_MIN, -big - 1, big + 1, LLONG_MAX}) {
        bool threw = false;
        try { toolkit::segments_intersect({0, 0}, {1, 0}, {bad, 0}, {1, 1}); }
        catch (const out_of_range&) { threw = true; }
        require(threw, "invalid segment coordinate=" + to_string(bad));
    }
    success();
}
