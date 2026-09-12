#include "toolkit/dag_shortest_paths.hpp"

// Any directed cycle, including an unreachable one, returns nullopt.
// Output: -2 unreachable
int main() {
    auto d = toolkit::dag_shortest_paths(4, {{0, 1, 3}, {1, 2, -5}, {0, 2, 9}}, 0);
    cout << *(*d)[2].value << ' ';
    if ((*d)[3].state == toolkit::DistanceState::unreachable) cout << "unreachable\n";
}
