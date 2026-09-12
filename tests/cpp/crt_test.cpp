#include <atcoder/math.hpp>
#include "test_util.hpp"

long long normalized(long long r, long long m) {
    r %= m;
    return r < 0 ? r + m : r;
}

void check(const vector<long long>& residues, const vector<long long>& moduli) {
    ++cases_checked;
    long long period = 1;
    for (long long m : moduli) period = lcm(period, m);
    pair<long long, long long> expected{0, 0};
    for (long long x = 0; x < period; ++x) {
        bool valid = true;
        for (int i = 0; i < int(moduli.size()); ++i)
            if (x % moduli[i] != normalized(residues[i], moduli[i])) valid = false;
        if (valid) { expected = {x, period}; break; }
    }
    require(atcoder::crt(residues, moduli) == expected,
            "r=" + show(residues) + " m=" + show(moduli));
}

int main() {
    check({}, {});
    check({0, 1}, {2, 2});
    check({2, 8}, {6, 9});
    check({-1, -3}, {4, 6});
    check({LLONG_MIN, LLONG_MAX}, {1, 1});
    check({LLONG_MIN, LLONG_MAX}, {7, 9});
    for (int t = 0; t < 300; ++t) {
        vector<long long> residues(rng() % 6), moduli(residues.size());
        for (int i = 0; i < int(moduli.size()); ++i) {
            moduli[i] = 1 + rng() % 10;
            residues[i] = int(rng() % 101) - 50;
        }
        check(residues, moduli);
    }
    ++cases_checked;
    require(atcoder::crt({-1}, {LLONG_MAX}) == make_pair(LLONG_MAX - 1, LLONG_MAX),
            "r=[-1] m=[LLONG_MAX]");
    ++cases_checked;
    const long long a = 3037000499LL, b = 3037000498LL;
    require(atcoder::crt({-1, -1}, {a, b}) == make_pair(a * b - 1, a * b),
            "r=[-1,-1] m=[3037000499,3037000498] bounded near-LLONG_MAX lcm");
    success();
}
