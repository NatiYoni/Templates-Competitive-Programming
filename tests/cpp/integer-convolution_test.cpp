#include <atcoder/convolution.hpp>
#include "test_util.hpp"

void check(const vector<long long>& a, const vector<long long>& b) {
    ++cases_checked;
    string input = "a=" + show(a) + " b=" + show(b);
    vector<__int128> wide;
    if (!a.empty() && !b.empty()) {
        wide.assign(a.size() + b.size() - 1, 0);
        for (int i = 0; i < int(a.size()); ++i)
            for (int j = 0; j < int(b.size()); ++j)
                wide[i+j] += __int128(a[i]) * b[j];
    }
    vector<long long> expected;
    for (__int128 x : wide) {
        require(LLONG_MIN <= x && x <= LLONG_MAX, input + " oracle coefficient overflow");
        expected.push_back(static_cast<long long>(x));
    }
    require(atcoder::convolution_ll(a, b) == expected, input);
}

int main() {
    check({}, {});
    check({}, {1});
    check({1}, {});
    check({LLONG_MIN, LLONG_MAX}, {1});
    check({-3000000000LL}, {3000000000LL});
    check({-1, 2, -3}, {4, -5});
    for (int t = 0; t < 260; ++t) {
        vector<long long> a(rng() % 41), b(rng() % 41);
        for (auto& x : a) x = int(rng() % 20001) - 10000;
        for (auto& x : b) x = int(rng() % 20001) - 10000;
        check(a, b);
    }
    for (auto lengths : {pair<int,int>{60, 69}, {61, 67}, {64, 65}, {65, 65},
                         {127, 129}, {128, 129}, {129, 129}, {256, 257}}) {
        vector<long long> a(lengths.first), b(lengths.second);
        for (auto& x : a) x = int(rng() % 2000001) - 1000000;
        for (auto& x : b) x = int(rng() % 2000001) - 1000000;
        check(a, b);
        fill(a.begin(), a.end(), 0);
        check(a, b);
    }
    success();
}
