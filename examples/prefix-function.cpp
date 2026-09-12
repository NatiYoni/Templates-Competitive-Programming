#include "toolkit/kactl_prelude.hpp"
#include "content/strings/KMP.h"

// KACTL pi(s): proper-prefix lengths, including {} for empty s, O(|s|) time/space.
// match(text, pattern): zero-based overlapping starts, O(|text|+|pattern|).
// Search usage requires a nonempty pattern and no NUL in either string: NUL is its separator.
// Raw match with empty pattern returns 1..|text|, not conventional 0..|text|.
// Input below: text ABABABA, pattern ABA. Output:
// 0 0 1 2 3 4 5
// 0 2 4
int main() {
    string text = "ABABABA", pattern = "ABA";
    auto prefixes = pi(text), occurrences = match(text, pattern);
    for (int i = 0; i < sz(prefixes); ++i) cout << (i ? " " : "") << prefixes[i];
    cout << '\n';
    for (int i = 0; i < sz(occurrences); ++i) cout << (i ? " " : "") << occurrences[i];
    cout << '\n';
}
