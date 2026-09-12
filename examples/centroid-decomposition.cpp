#include "toolkit/centroid_nearest.hpp"

// Nyaan centroid hierarchy (CC0), original nearest-marked application.
// Nonempty unweighted tree; no deactivation except reset(). Output: 3 1
int main() {
    toolkit::CentroidNearest tree(4, {{0, 1}, {1, 2}, {2, 3}});
    tree.activate(0);
    cout << *tree.nearest(3) << ' ';
    tree.activate(2);
    cout << *tree.nearest(3) << '\n';
}
