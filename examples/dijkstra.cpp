#include "toolkit/dijkstra.hpp"

// Input (fixed): 0->1 cost 8, 0->2 cost 2, 2->1 cost 3; vertex 3 isolated.
// Source 0. Expected output (-1 printed for LLONG_MAX): 0 5 2 -1
int main() {
    auto distance = toolkit::dijkstra({{{1, 8}, {2, 2}}, {}, {{1, 3}}, {}}, 0);
    for (size_t i = 0; i < distance.size(); ++i)
        cout << (i ? " " : "") << (distance[i] == LLONG_MAX ? -1 : distance[i]);
    cout << '\n';
}
