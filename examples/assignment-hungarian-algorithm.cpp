#include "toolkit/hungarian_assignment.hpp"

// Luzhiled Hungarian/Matrix, Unlicense; original checked zero-based adapter.
// Output: -3 1 0 (cost, assigned column of row 0, assigned column of row 1).
int main() {
    auto result = toolkit::hungarian_assignment({{5, -2, 4}, {-1, 3, 7}});
    cout << result.cost << ' ' << result.column[0] << ' ' << result.column[1] << '\n';
}
