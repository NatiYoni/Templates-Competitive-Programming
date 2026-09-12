#include "toolkit/base.hpp"
#include <atcoder/lazysegtree.hpp>
#include <atcoder/modint.hpp>

// ACL lazy_segtree + modint (CC0). Store (sum,length) for range affine updates
// x <- a*x+b modulo 998244353. Real leaves MUST have length 1, identity length 0.
// composition(f,g) means f after g. Points are zero-based, ranges [l,r);
// n >= 0 and lengths fit int (ACL n <= 1e8). Build/memory O(n), updates/range
// queries O(log(n+1)), all_prod O(1). Queries may push lazy state.
// Modular sums are NOT monotone: use lengths, not modular sums, for searches.
using Mint = atcoder::modint998244353;
struct Segment { Mint sum; int length; };
struct Affine { Mint a, b; };
Segment combine(Segment x, Segment y) {
    return {x.sum + y.sum, x.length + y.length};
}
Segment empty_segment() { return {0, 0}; }
Segment mapping(Affine f, Segment x) {
    return {f.a * x.sum + f.b * x.length, x.length};
}
Affine composition(Affine f, Affine g) {
    return {f.a * g.a, f.a * g.b + f.b};
}
Affine identity_map() { return {1, 0}; }
using AffineTree = atcoder::lazy_segtree<Segment, combine, empty_segment,
                                       Affine, mapping, composition, identity_map>;

#ifndef TOOLKIT_EXAMPLE_NO_MAIN
// Input [1,2,3,4]; affine [1,4) by 2*x+1, point 0 by 3*x, set point 3 to 9.
// Output: 3 15 24 2 2
int main() {
    AffineTree tree(vector<Segment>{{1, 1}, {2, 1}, {3, 1}, {4, 1}});
    tree.apply(1, 4, Affine{2, 1});
    tree.apply(0, Affine{3, 0});
    tree.set(3, Segment{9, 1});
    auto at_most_two = [](Segment x) { return x.length <= 2; };
    cout << tree.get(0).sum.val() << ' ' << tree.prod(0, 3).sum.val() << ' '
         << tree.all_prod().sum.val() << ' ' << tree.max_right(0, at_most_two)
         << ' ' << tree.min_left(4, at_most_two) << '\n';
}
#endif
