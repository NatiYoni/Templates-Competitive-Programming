#include "toolkit/floyd_warshall.hpp"

// Output: -1 unreachable -inf
int main() {
    auto d = toolkit::floyd_warshall(4, {{0, 1, 2}, {1, 2, -3}, {3, 3, -1}});
    cout << *d[0][2].value << ' ';
    if (d[2][0].state == toolkit::DistanceState::unreachable) cout << "unreachable ";
    if (d[3][3].state == toolkit::DistanceState::negative_infinity) cout << "-inf\n";
}
