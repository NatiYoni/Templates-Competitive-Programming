#include "toolkit/base.hpp"
#include <atcoder/dsu.hpp>

// ACL dsu (CC0): vertices [0,n); n >= 0. merge/same/size are amortized
// O(alpha(n)); groups is O(n alpha(n)); memory O(n). Queries compress paths.
// Demonstration input: five isolated vertices, then edges 0-1 and 1-2.
// Output: 1 3 3
int main() {
    atcoder::dsu components(5);
    components.merge(0, 1);
    components.merge(1, 2);
    cout << components.same(0, 2) << ' ' << components.size(1) << ' '
         << components.groups().size() << '\n';
}
