#include "toolkit/rerooting.hpp"

// Independent original ordered associative framework, here (count,distance sum).
// Output: 3 2 3 (sum of unweighted distances from each root).
int main() {
    using State = pair<long long, long long>;
    auto answer = toolkit::rerooting(3, {{0, 1}, {1, 2}}, State{0, 0},
        [](State a, State b) { return State{a.first + b.first, a.second + b.second}; },
        [](State a, int) { return State{a.first + 1, a.second}; },
        [](State a, int, int) { return State{a.first, a.second + a.first}; });
    for (int v = 0; v < 3; ++v) cout << (v ? " " : "") << answer[v].second;
    cout << '\n';
}
