#include "toolkit/base.hpp"
#include <atcoder/segtree.hpp>

// ACL segtree (CC0), sum monoid: identity 0, associative bounded long long sums.
// Zero-based points and half-open ranges; n >= 0. set/get/prod/all_prod expose
// the actual API. For sum <= limit boundary searches, values and limit must
// be nonnegative, so the predicate is monotone and accepts the identity.
// Build O(n), get/all_prod O(1), other operations O(log(n+1)), memory O(n).
long long sum_op(long long a, long long b) { return a + b; }
long long sum_identity() { return 0; }
using SumTree = atcoder::segtree<long long, sum_op, sum_identity>;

#ifndef TOOLKIT_EXAMPLE_NO_MAIN
// Input [2,1,3,4], set(1,5), boundary limit 8. Output: 5 10 14 2 2
int main() {
    SumTree tree(vector<long long>{2, 1, 3, 4});
    tree.set(1, 5);
    auto within = [](long long sum) { return sum <= 8; };
    cout << tree.get(1) << ' ' << tree.prod(0, 3) << ' ' << tree.all_prod()
         << ' ' << tree.max_right(0, within) << ' ' << tree.min_left(4, within)
         << '\n';
}
#endif
