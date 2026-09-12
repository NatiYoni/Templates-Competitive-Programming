#include "toolkit/longest_common_subsequence.hpp"

// Repository-original byte-string DP with actual subsequence reconstruction.
// O(n*m) time/table space; either empty -> ""; lengths <INT_MAX, table must fit
// memory. Ties prefer removing a's last byte; not a lexicographic-minimum promise.
// Input abcde / ace. Output: 3 ace.
int main() {
    auto subsequence = toolkit::longest_common_subsequence("abcde", "ace");
    cout << subsequence.size() << ' ' << subsequence << '\n';
}
