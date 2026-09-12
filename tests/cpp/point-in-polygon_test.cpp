#include "toolkit/point_in_polygon.hpp"
#include "test_util.hpp"

using Q = toolkit::IntegerPoint;
using Location = toolkit::PolygonLocation;
using Wide = __int128;

// Independent signed winding-number reference, not the upstream parity test.
Location winding(const vector<Q>& polygon, Q p) {
    int winding_number = 0;
    for (size_t i = 0; i < polygon.size(); ++i) {
        Q a = polygon[i], b = polygon[(i + 1) % polygon.size()];
        Wide dx = Wide(b.x) - a.x, dy = Wide(b.y) - a.y;
        Wide px = Wide(p.x) - a.x, py = Wide(p.y) - a.y;
        Wide determinant = dx * py - dy * px;
        if (determinant == 0 && min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x) &&
            min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y)) return Location::boundary;
        if (a.y <= p.y && b.y > p.y && determinant > 0) ++winding_number;
        if (a.y > p.y && b.y <= p.y && determinant < 0) --winding_number;
    }
    return winding_number ? Location::inside : Location::outside;
}
void check(const vector<Q>& polygon, Q p) {
    ++cases_checked;
    string input = "polygon=" + show(polygon) + " query=" + show(vector<Q>{p});
    auto expected = winding(polygon, p);
    auto copy = polygon;
    require(toolkit::point_in_polygon(polygon, p) == expected, input + " signed winding oracle");
    require(polygon == copy, input + " unchanged input");
    reverse(copy.begin(), copy.end());
    require(toolkit::point_in_polygon(copy, p) == expected, input + " reversed orientation");
    if (!copy.empty()) {
        copy.push_back(copy.front());
        require(toolkit::point_in_polygon(copy, p) == expected, input + " repeated closing vertex");
    }
}
int main() {
    vector<vector<Q>> fixed{
        {}, {{0, 0}}, {{0, 0}, {0, 0}}, {{-2, 0}, {2, 0}},
        {{-2, 0}, {0, 0}, {2, 0}}, {{0, 0}, {4, 0}, {0, 4}},
        {{0, 0}, {4, 0}, {4, 4}, {0, 4}},
        {{0, 0}, {4, 0}, {4, 4}, {2, 2}, {0, 4}},
        {{0, 0}, {4, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}}
    };
    for (const auto& polygon : fixed)
        for (int x = -3; x <= 5; ++x) for (int y = -3; y <= 5; ++y) check(polygon, {x, y});
    const long long big = 1000000000000000000LL;
    vector<Q> large{{-big, -big}, {big, -big}, {big, big}, {-big, big}};
    for (Q p : vector<Q>{{0, 0}, {big, 0}, {-big, -big}}) check(large, p);
    check({{-big, -big}, {big, -big}, {big, big}}, {-big, big});
    for (int t = 0; t < 350; ++t) {
        int count = 2 + rng() % 9;
        long long scale = t % 3 ? 2 : 10000000000000000LL;
        vector<Q> polygon;
        for (int i = 0; i < count; ++i)
            polygon.push_back({(i - 5) * scale, (1 + int(rng() % 20)) * scale});
        for (int i = count - 1; i >= 0; --i)
            polygon.push_back({(i - 5) * scale, -(1 + int(rng() % 20)) * scale});
        for (int q = 0; q < 5; ++q)
            check(polygon, {(int(rng() % 25) - 12) * scale, (int(rng() % 51) - 25) * scale});
        Q a = polygon[rng() % polygon.size()];
        check(polygon, a);
        Q b = polygon[1];
        check(polygon, {(polygon[0].x + b.x) / 2, (polygon[0].y + b.y) / 2});
    }
    for (Q bad : vector<Q>{{LLONG_MIN, 0}, {0, LLONG_MAX}, {big + 1, 0}}) {
        bool query_threw = false, vertex_threw = false;
        try { toolkit::point_in_polygon({}, bad); } catch (const out_of_range&) { query_threw = true; }
        try { toolkit::point_in_polygon({bad}, {0, 0}); } catch (const out_of_range&) { vertex_threw = true; }
        require(query_threw && vertex_threw, "invalid polygon point=" + show(vector<Q>{bad}));
    }
    success();
}
