#include "toolkit/heavy_light.hpp"

// Luzhiled layout/LCA, Unlicense; original ordered half-open segment adapter.
// Output: 3 1 0 2 (vertex order along the path, not commutative aggregation).
int main() {
    toolkit::HeavyLight hld(4, {{0, 1}, {0, 2}, {1, 3}});
    vector<int> path;
    for (auto s : hld.path(3, 2)) {
        if (s.reversed) for (int i = s.last; i-- > s.first;)
            path.push_back(hld.vertex_at()[i]);
        else for (int i = s.first; i < s.last; ++i)
            path.push_back(hld.vertex_at()[i]);
    }
    for (size_t i = 0; i < path.size(); ++i) cout << (i ? " " : "") << path[i];
    cout << '\n';
}
