#include "toolkit/kactl_prelude.hpp"
#include "content/strings/AhoCorasick.h"

// KACTL alphabet is exactly uppercase A-Z; patterns nonempty, duplicates allowed.
// find(text)[end] gives the longest ending pattern ID (-1 if none); duplicates use
// the last inserted ID. findAll(original_patterns,text)[start] returns ALL IDs,
// shortest first (duplicate IDs reverse insertion order). Do not call insert
// after construction or change patterns before findAll. Empty text/list is valid.
// Build O(26*total pattern length); find O(text length); findAll O(text+matches).
// Output for patterns HE,SHE,HERS,HE and text SHEHERS:
// -1 -1 1 -1 3 -1 2
// 0: 1
// 1: 3 0
// 3: 3 0 2
int main() {
    vector<string> patterns{"HE", "SHE", "HERS", "HE"};
    string text = "SHEHERS";
    AhoCorasick ac(patterns);
    auto longest = ac.find(text);
    for (int i = 0; i < sz(longest); ++i) cout << (i ? " " : "") << longest[i];
    cout << '\n';
    auto matches = ac.findAll(patterns, text);
    for (int i = 0; i < sz(matches); ++i) if (!matches[i].empty()) {
        cout << i << ':';
        for (int id : matches[i]) cout << ' ' << id;
        cout << '\n';
    }
}
