#include "toolkit/base.hpp"
#include <atcoder/string.hpp>

// ACL lcp_array(s,sa)[i] = LCP(s[sa[i]..],s[sa[i+1]..]), O(n) time/space.
// s MUST be nonempty; sa must be the valid sorted permutation of [0,n).
// Singleton returns {}; neither input changes. ASCII suffix_array used below.
// Input banana; adjacent suffix LCP output: 1 3 0 0 2
int main() {
    string text = "banana";
    auto sa = atcoder::suffix_array(text);
    auto lcp = atcoder::lcp_array(text, sa);
    for (int i = 0; i < int(lcp.size()); ++i) cout << (i ? " " : "") << lcp[i];
    cout << '\n';
}
