#include <atcoder/convolution.hpp>
#include <atcoder/modint.hpp>
#include "test_util.hpp"

constexpr long long prime_modulus = 998244353;
long long normalized(long long x) {
    x %= prime_modulus;
    return x < 0 ? x + prime_modulus : x;
}

void check(const vector<long long>& a, const vector<long long>& b) {
    ++cases_checked;
    vector<long long> expected;
    if (!a.empty() && !b.empty()) {
        expected.assign(a.size() + b.size() - 1, 0);
        for (int i = 0; i < int(a.size()); ++i)
            for (int j = 0; j < int(b.size()); ++j)
                expected[i+j] = (expected[i+j] + normalized(a[i]) * normalized(b[j])) % prime_modulus;
    }
    string input = "a=" + show(a) + " b=" + show(b);
    require(atcoder::convolution(a, b) == expected, input + " integral overload");
    using Mint = atcoder::modint998244353;
    vector<Mint> ma(a.begin(), a.end()), mb(b.begin(), b.end());
    auto product = atcoder::convolution(ma, mb);
    vector<long long> values;
    for (Mint x : product) values.push_back(x.val());
    require(values == expected, input + " modint overload");
    require(atcoder::convolution(std::move(ma), std::move(mb)) == product, input + " rvalue overload");
}

int main() {
    check({}, {});
    check({}, {1, 2});
    check({0}, {});
    check({LLONG_MIN, LLONG_MAX}, {-1, 0, 1});
    for (int t = 0; t < 260; ++t) {
        vector<long long> a(rng() % 41), b(rng() % 41);
        for (auto& x : a) x = int(rng() % 2001) - 1000;
        for (auto& x : b) x = int(rng() % 2001) - 1000;
        check(a, b);
    }
    // min(n,m)>60 forces NTT; output lengths straddle power-of-two transform boundaries.
    for (auto lengths : {pair<int,int>{60, 69}, {61, 67}, {64, 65}, {65, 65},
                         {127, 129}, {128, 129}, {129, 129}, {256, 257}}) {
        vector<long long> a(lengths.first), b(lengths.second);
        for (auto& x : a) x = rng() % prime_modulus;
        for (auto& x : b) x = rng() % prime_modulus;
        check(a, b);
        fill(a.begin(), a.end(), 0);
        check(a, b);
    }
    success();
}
