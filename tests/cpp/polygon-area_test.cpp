#include "toolkit/kactl_prelude.hpp"
#include "content/geometry/PolygonArea.h"
#include "test_util.hpp"

using P = Point<long long>;

void check(vector<P> polygon) {
    ++cases_checked;
    string input = "polygon=" + show(polygon);
    __int128 expected = 0;
    // Triangulation about the first vertex, independent of upstream edge summation.
    for (int i = 1; i + 1 < int(polygon.size()); ++i)
        expected += __int128(polygon[i].x - polygon[0].x) * (polygon[i+1].y - polygon[0].y)
                  - __int128(polygon[i].y - polygon[0].y) * (polygon[i+1].x - polygon[0].x);
    require(LLONG_MIN <= expected && expected <= LLONG_MAX, input + " oracle range");
    auto before = polygon;
    long long area = polygonArea2(polygon);
    require(area == static_cast<long long>(expected) && polygon == before, input + " triangulation");
    reverse(polygon.begin(), polygon.end());
    require(polygonArea2(polygon) == -area, input + " reversed orientation");
    for (P& point : polygon) { point.x += 37; point.y -= 23; }
    require(polygonArea2(polygon) == -area, input + " translation invariance");
}

int main() {
    // polygonArea2 accesses back(): the empty polygon is outside its contract.
    check({P(0, 0)});
    check({P(1, 2), P(3, 4)});
    check({P(1, 1), P(1, 1), P(1, 1)});
    check({P(0, 0), P(1, 1), P(2, 2)});
    check({P(0, 0), P(3, 0), P(3, 2), P(1, 1), P(0, 2)});
    check({P(-100000000, -100000000), P(100000000, -100000000),
           P(100000000, 100000000), P(-100000000, 100000000)});
    for (int t = 0; t < 300; ++t) {
        long long w = 1 + rng() % 1000, h = 1 + rng() % 1000;
        vector<P> rectangle{P(0, 0), P(w, 0), P(w, h), P(0, h)};
        check(rectangle);
        ++cases_checked;
        require(polygonArea2(rectangle) == 2*w*h, "rectangle width=" + to_string(w) + " height=" + to_string(h));
        // Arbitrary ordered chains test signed algebraic area, not union area.
        vector<P> chain;
        for (int i = 0, n = 1 + rng() % 15; i < n; ++i)
            chain.emplace_back(int(rng() % 41)-20, int(rng() % 41)-20);
        check(chain);
    }
    success();
}
