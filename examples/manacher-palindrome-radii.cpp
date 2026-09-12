#include "toolkit/palindrome_radii.hpp"

// Nyaan's unchanged CC0 Manacher plus repository-original byte-safe adapter.
// O(n) time/space; no recursion or input mutation. n<=(INT_MAX-3)/4.
// odd includes its center; even[i] is centered BEFORE i; empty -> two empty arrays.
// Input: abba. Output odd: 1 1 1 1; even: 0 0 2 0.
int main() {
    auto radii = toolkit::palindrome_radii("abba");
    for (int i = 0; i < 4; ++i) cout << (i ? " " : "") << radii.odd[i];
    cout << '\n';
    for (int i = 0; i < 4; ++i) cout << (i ? " " : "") << radii.even[i];
    cout << '\n';
}
