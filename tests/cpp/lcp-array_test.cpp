#include <atcoder/string.hpp>
#include "test_util.hpp"

void check(const string& s) {
    ++cases_checked;
    vector<pair<string, int>> suffixes;
    for (int i = 0; i < int(s.size()); ++i) suffixes.push_back({s.substr(i), i});
    sort(suffixes.begin(), suffixes.end());
    vector<int> sa, expected(s.size() - 1);
    for (const auto& suffix : suffixes) sa.push_back(suffix.second);
    for (int i = 0; i + 1 < int(s.size()); ++i) {
        const string& a = suffixes[i].first;
        const string& b = suffixes[i + 1].first;
        while (expected[i] < int(min(a.size(), b.size())) && a[expected[i]] == b[expected[i]])
            ++expected[i];
    }
    string input = "s=" + show(vector<int>(s.begin(), s.end())) + " sa=" + show(sa);
    require(atcoder::lcp_array(s, sa) == expected, input);
    vector<int> values(s.begin(), s.end());
    for (int& x : values) x -= 200;
    require(atcoder::lcp_array(values, sa) == expected, "values=" + show(values) + " sa=" + show(sa));
}

int main() {
    // Empty input is outside the upstream lcp_array contract; singleton gives {}.
    for (string s : {"A", "banana", "AAAAAA", "ABABABA", "ZYXW"}) check(s);
    check(string("\0A\0A", 4));
    for (int t = 0; t < 300; ++t) {
        string s(1 + rng() % 100, 'A');
        for (char& c : s) c += rng() % 5;
        check(s);
    }
    success();
}
