#include <atcoder/string.hpp>
#include "test_util.hpp"

template<class T> vector<int> explicit_suffix_sort(const vector<T>& s) {
    vector<pair<vector<T>, int>> suffixes;
    for (int i = 0; i < int(s.size()); ++i)
        suffixes.push_back({vector<T>(s.begin() + i, s.end()), i});
    sort(suffixes.begin(), suffixes.end());
    vector<int> expected;
    for (const auto& suffix : suffixes) expected.push_back(suffix.second);
    return expected;
}

void check(const string& s) {
    ++cases_checked;
    vector<int> bytes(s.begin(), s.end());
    auto expected = explicit_suffix_sort(bytes);
    require(atcoder::suffix_array(s) == expected, "ASCII=" + show(bytes));
    require(atcoder::suffix_array(bytes, 127) == expected, "bounded=" + show(bytes));
    vector<long long> signed_values(bytes.begin(), bytes.end());
    for (auto& x : signed_values) x = x * 1000000000000LL - 9000000000000LL;
    require(atcoder::suffix_array(signed_values) == expected, "generic=" + show(signed_values));
}

int main() {
    for (int n : {0, 1, 2, 9, 10, 39, 40, 41, 128}) {
        check(string(n, 'A'));
        string s(n, '\0');
        for (int i = 0; i < n; ++i) s[i] = char(i % 128);
        check(s);
    }
    check("banana");
    ++cases_checked;
    vector<int> high_bytes{255, 128, 0, 255, 128, 127};
    require(atcoder::suffix_array(high_bytes, 255) == explicit_suffix_sort(high_bytes),
            "unsigned byte vector=" + show(high_bytes));
    for (int t = 0; t < 300; ++t) {
        string s(rng() % 100, 'A');
        for (char& c : s) c += rng() % 4;
        check(s);
    }
    success();
}
