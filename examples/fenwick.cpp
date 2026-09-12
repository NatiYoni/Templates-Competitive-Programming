#include "toolkit/base.hpp"
#include <atcoder/fenwicktree.hpp>

// ACL fenwick_tree (CC0): point add on [0,n), sum on [l,r), including empty.
// n >= 0; keep mathematical sums in long long. Signed negative updates work
// through ACL's unsigned internal accumulation on this GNU C++17 profile.
// Construction O(n), each operation O(log(n+1)), memory O(n).
// Demonstration input: [2,1,4,3], then add(1,5). Output: 13 0
int main() {
    atcoder::fenwick_tree<long long> sums(4);
    vector<long long> values{2, 1, 4, 3};
    for (int i = 0; i < 4; ++i) sums.add(i, values[i]);
    sums.add(1, 5);
    cout << sums.sum(1, 4) << ' ' << sums.sum(2, 2) << '\n';
}
