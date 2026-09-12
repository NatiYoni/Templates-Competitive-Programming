#include "toolkit/eulerian_walk.hpp"

// Luzhiled, Unlicense. false selects undirected edges; true selects directed.
// Output: 3 0 2 (number of edges, first vertex, last vertex).
int main() {
    auto walk = toolkit::eulerian_walk<false>(3, {{0, 1}, {1, 1}, {1, 2}});
    if (walk) cout << walk->edges.size() << ' ' << walk->vertices.front() << ' '
                   << walk->vertices.back() << '\n';
}
