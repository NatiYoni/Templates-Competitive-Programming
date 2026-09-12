#include <atcoder/string.hpp>
#include "test_util.hpp"

void check(const string& s) {
    ++cases_checked;
    vector<int> expected(s.size());
    for (int i = 0; i < int(s.size()); ++i)
        while (i + expected[i] < int(s.size()) && s[expected[i]] == s[i + expected[i]])
            ++expected[i];
    require(atcoder::z_algorithm(s) == expected, "s=" + show(vector<int>(s.begin(), s.end())));
    vector<int> values(s.begin(), s.end());
    for (int& x : values) x = 3 * x - 700;
    require(atcoder::z_algorithm(values) == expected, "values=" + show(values));
}

int main() {
    for (string s : {"", "A", "AAAAAA", "ABABABA", "ABCDEF"}) check(s);
    check(string("\0A\0A", 4));
    for (int t = 0; t < 300; ++t) {
        string s(rng() % 100, 'A');
        for (char& c : s) c += rng() % 5;
        check(s);
    }
    success();
}
