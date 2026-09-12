#include "toolkit/closest_pair.hpp"
#include "test_util.hpp"

using Q = toolkit::IntegerPoint;
__int128 distance_squared(Q a, Q b) {
    __int128 dx = __int128(a.x) - b.x, dy = __int128(a.y) - b.y;
    return dx * dx + dy * dy;
}
void verify_pair(const vector<Q>& points, long long expected, const string& input) {
    auto actual = toolkit::closest_pair(points);
    require(bool(actual), input + " missing pair");
    require(actual->squared_distance == expected, input + " expected distance2=" + to_string(expected) +
            " actual=" + to_string(actual->squared_distance));
    bool distinct_entries = false;
    auto first = find(points.begin(), points.end(), actual->first);
    if (first != points.end()) {
        for (auto it = points.begin(); it != points.end(); ++it)
            if (it != first && *it == actual->second) { distinct_entries = true; break; }
    }
    require(distinct_entries && distance_squared(actual->first, actual->second) == expected,
            input + " returned positions=" + show(vector<Q>{actual->first, actual->second}));
}
void check(const vector<Q>& points) {
    ++cases_checked;
    string input = "points=" + show(points);
    auto copy = points;
    if (points.size() < 2) {
        require(!toolkit::closest_pair(points), input + " fewer than two");
    } else {
        __int128 best = __int128(LLONG_MAX) * LLONG_MAX;
        for (size_t i = 0; i < points.size(); ++i)
            for (size_t j = 0; j < i; ++j) best = min(best, distance_squared(points[i], points[j]));
        verify_pair(points, (long long)best, input);
    }
    require(points == copy, input + " unchanged caller input");
}
int main() {
    check({});
    check({{0, 0}});
    check({{1, 2}, {1, 2}});
    check({{0, 0}, {3, 4}});
    check({{-100000000, -100000000}, {100000000, 100000000}});
    check({{-100000000, -100000000}, {-100000000, 100000000},
           {100000000, -100000000}, {100000000, 100000000}});
    check({{1, 1}, {1, 100}, {1, -100}, {1, 2}});
    for (int t = 0; t < 500; ++t) {
        vector<Q> points(rng() % 65);
        long long scale = t % 4 ? 1 : 1000000;
        for (auto& p : points)
            p = {(int(rng() % 201) - 100) * scale, (int(rng() % 201) - 100) * scale};
        if (t % 7 == 0) for (auto& p : points) p.x = 0;
        if (t % 11 == 0) for (auto& p : points) p.y = 0;
        if (t % 5 == 0 && points.size() >= 2) points.back() = points.front();
        shuffle(points.begin(), points.end(), rng);
        check(points);
    }
    vector<Q> large;
    for (int i = 0; i < 100000; ++i) large.push_back({0, i});
    shuffle(large.begin(), large.end(), rng);
    ++cases_checked;
    verify_pair(large, 1, "100000 unique vertical points (0,i), i=0..99999, seeded shuffle");
    large.assign(100000, {7, 8});
    ++cases_checked;
    verify_pair(large, 0, "100000 duplicate points (7,8)");
    for (Q bad : vector<Q>{{100000001, 0}, {0, -100000001}, {LLONG_MIN, LLONG_MAX}}) {
        bool threw = false;
        try { toolkit::closest_pair({bad}); } catch (const out_of_range&) { threw = true; }
        require(threw, "invalid closest-pair singleton coordinate=" + show(vector<Q>{bad}));
    }
    success();
}
