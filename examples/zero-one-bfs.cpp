#include "toolkit/zero_one_bfs.hpp"

// Input (fixed): 0->1 cost 1, 0->2 cost 0, 2->1 cost 0; vertex 3 isolated.
// Source 0. Expected output (-1 printed for LLONG_MAX): 0 0 0 -1
int main() {
    auto distance = toolkit::zero_one_bfs({{{1, 1}, {2, 0}}, {}, {{1, 0}}, {}}, 0);
    for (size_t i = 0; i < distance.size(); ++i)
        cout << (i ? " " : "") << (distance[i] == LLONG_MAX ? -1 : distance[i]);
    cout << '\n';
}
