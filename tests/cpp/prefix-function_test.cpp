#include "toolkit/kactl_prelude.hpp"
#include "content/strings/KMP.h"
#include "test_util.hpp"

void check(const string& text, const string& pattern) {
    ++cases_checked;
    string input = "text=" + text + " pattern=" + pattern;
    vi expected(text.size());
    for (int i = 0; i < int(text.size()); ++i)
        for (int k = 1; k <= i; ++k)
            if (text.substr(0, k) == text.substr(i - k + 1, k)) expected[i] = k;
    require(pi(text) == expected, input + " prefix");
    vi positions;
    if (pattern.empty()) {
        // Raw KACTL match omits boundary zero for an empty, NUL-free pattern.
        for (int i = 1; i <= int(text.size()); ++i) positions.push_back(i);
    } else {
        for (int i = 0; i + int(pattern.size()) <= int(text.size()); ++i)
            if (text.substr(i, pattern.size()) == pattern) positions.push_back(i);
    }
    require(match(text, pattern) == positions, input + " search");
}

int main() {
    check("", "");
    check("", "A");
    check("ABC", "");
    check("AAAAA", "AA");
    check("ABABABA", "ABA");
    check("A", "LONGER");
    for (int t = 0; t < 300; ++t) {
        string text(rng() % 65, 'A'), pattern(rng() % 17, 'A');
        for (char& c : text) c += rng() % 4;
        for (char& c : pattern) c += rng() % 4;
        check(text, pattern);
    }
    success();
}
