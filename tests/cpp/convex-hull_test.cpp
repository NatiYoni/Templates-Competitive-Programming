#include "toolkit/kactl_prelude.hpp"
#include "content/geometry/ConvexHull.h"
#include "test_util.hpp"

long long turn(P a, P b, P c) {
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}
long long distance2(P a, P b) {
    return (b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y);
}

vector<P> gift_wrap(vector<P> points) {
    sort(points.begin(), points.end());
    points.erase(unique(points.begin(), points.end()), points.end());
    if (points.size() < 2) return points;
    vector<P> hull;
    int current = 0;
    do {
        hull.push_back(points[current]);
        int next = (current + 1) % points.size();
        for (int j = 0; j < int(points.size()); ++j) if (j != current) {
            long long orientation = turn(points[current], points[next], points[j]);
            if (orientation < 0 || (orientation == 0 &&
                distance2(points[current], points[j]) > distance2(points[current], points[next])))
                next = j;
        }
        current = next;
    } while (current != 0);
    return hull;
}

void check(const vector<P>& points) {
    ++cases_checked;
    auto actual = convexHull(points), expected = gift_wrap(points);
    string input = "points=" + show(points);
    require(actual == expected, input + " hull=" + show(actual) + " gift-wrap=" + show(expected));
    if (actual.size() >= 3)
        for (int i = 0; i < int(actual.size()); ++i) {
            P a = actual[i], b = actual[(i+1) % actual.size()];
            require(turn(a, b, actual[(i+2) % actual.size()]) > 0, input + " strict CCW");
            for (P p : points) require(turn(a, b, p) >= 0, input + " containment");
        }
}

int main() {
    check({});
    check({P(1, 2)});
    check({P(1, 2), P(1, 2), P(1, 2)});
    check({P(0, 0), P(1, 1), P(2, 2), P(-1, -1), P(0, 0)});
    check({P(0, 0), P(0, 1), P(0, -1)});
    check({P(-100000000, -100000000), P(100000000, -100000000),
           P(100000000, 100000000), P(-100000000, 100000000), P(0, 0)});
    for (int t = 0; t < 300; ++t) {
        vector<P> points;
        int n = rng() % 31;
        for (int i = 0; i < n; ++i) {
            long long x = int(rng() % 21) - 10;
            long long y = t % 5 == 0 ? 2*x+1 : int(rng() % 21)-10;
            points.emplace_back(x, y);
        }
        check(points);
    }
    success();
}
