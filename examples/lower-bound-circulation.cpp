#include "toolkit/lower_bound_circulation.hpp"

// ACL maxflow (CC0), original lower-bound balance/recovery adapter.
// Output: 2 2 (flows in input edge order); nullopt would mean infeasible.
int main() {
    auto flow = toolkit::lower_bound_circulation(2, {{0, 1, 2, 3}, {1, 0, 0, 4}});
    if (flow) cout << (*flow)[0] << ' ' << (*flow)[1] << '\n';
}
