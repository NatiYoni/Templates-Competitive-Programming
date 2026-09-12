#include "toolkit/bellman_ford.hpp"

// Directed signed edges; non-finite states carry no numeric value.
// Output: 0 -inf -inf unreachable
int main() {
    auto d = toolkit::bellman_ford(4, {{0, 1, 2}, {1, 1, -1}, {1, 2, 3}}, 0);
    for (int v = 0; v < 4; ++v) {
        if (v) cout << ' ';
        if (d[v].value) cout << *d[v].value;
        else cout << (d[v].state == toolkit::DistanceState::negative_infinity
                      ? "-inf" : "unreachable");
    }
    cout << '\n';
}
