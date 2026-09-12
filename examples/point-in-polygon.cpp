#include "toolkit/point_in_polygon.hpp"

// Unchanged KACTL CC0 even-odd predicate with repository-original 3-way adapter.
// Simple cyclic polygon, either orientation; empty/singleton/segment supported.
// Exact boundary is a separate result, not silently counted as inside/outside.
// Checked |coordinate|<=1e18, widened BEFORE multiplication, O(n) time/space.
// Input square and interior/edge/exterior queries. Output: inside boundary outside.
int main() {
    vector<toolkit::IntegerPoint> square{{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    for (auto q : vector<toolkit::IntegerPoint>{{2, 2}, {4, 2}, {5, 2}}) {
        auto location = toolkit::point_in_polygon(square, q);
        cout << (location == toolkit::PolygonLocation::inside ? "inside" :
                 location == toolkit::PolygonLocation::boundary ? "boundary" : "outside")
             << (q.x == 5 ? "\n" : " ");
    }
}
