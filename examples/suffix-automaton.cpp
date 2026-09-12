#include "toolkit/suffix_automaton.hpp"

// Unchanged Suisen CC0 engine with repository-original immutable owning facade.
// All bytes (including NUL), overlapping occurrences; empty occurs n+1 times.
// Distinct count excludes empty. n<=(INT_MAX-1)/2; O(n log(256)) construction,
// O(n) storage, O(pattern length * log(256)) query, no algorithm recursion.
// Snapshot owns its state; copies remain valid independently, no append API.
// Input ababa: distinct=9, occurrences(aba)=2, occurrences(empty)=6.
int main() {
    toolkit::SuffixAutomaton index("ababa");
    cout << index.distinct_substrings() << ' ' << index.occurrences("aba") << ' '
         << index.occurrences("") << '\n';
}
