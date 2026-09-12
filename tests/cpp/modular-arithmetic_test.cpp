#include <atcoder/modint.hpp>
#include "test_util.hpp"

long long norm(long long value, int modulus) {
    long long r = value % modulus;
    return r < 0 ? r + modulus : r;
}

template<class Mint> void check(long long a, long long b, int exponent) {
    ++cases_checked;
    int m = Mint::mod();
    long long x = norm(a, m), y = norm(b, m), power = 1 % m;
    string input = "a=" + to_string(a) + " b=" + to_string(b) +
                   " modulus=" + to_string(m) + " exponent=" + to_string(exponent);
    Mint ma(a), mb(b);
    require(ma.val() == x && mb.val() == y && Mint().val() == 0, input + " normalization");
    require((ma + mb).val() == (x + y) % m, input + " addition");
    require((ma - mb).val() == norm(x - y, m), input + " subtraction");
    require((ma * mb).val() == x * y % m, input + " multiplication");
    require((-ma).val() == norm(-x, m), input + " unary minus");
    for (int i = 0; i < exponent; ++i) power = power * x % m;
    require(ma.pow(exponent).val() == power, input + " power");
    auto changed = ma;
    require((changed++).val() == x && changed.val() == (x + 1) % m, input + " increment");
    require((--changed).val() == x, input + " decrement");
    require(Mint::raw(int(x)).val() == x, input + " valid raw");
    if (gcd(y, 1LL * m) == 1) {
        auto inv = mb.inv();
        require(y * inv.val() % m == 1 % m, input + " inverse");
        if (m <= 257) {
            int expected = 0;
            while (1LL * expected * y % m != 1 % m) ++expected;
            require(inv.val() == expected, input + " enumerated inverse");
        }
        require((ma / mb).val() == x * inv.val() % m, input + " division");
    }
}

int main() {
    using Dynamic = atcoder::dynamic_modint<0>;
    for (int m : {1, 2, 12, 17, 998244353, 2000001000}) {
        Dynamic::set_mod(m);
        check<Dynamic>(LLONG_MIN, LLONG_MAX, 0);
        check<Dynamic>(-m, m - 1, 13);
        check<Dynamic>(LLONG_MAX, LLONG_MIN, 7);
    }
    for (int t = 0; t < 300; ++t) {
        long long a = int(rng() % 200001) - 100000, b = int(rng() % 200001) - 100000;
        int exponent = int(rng() % 20);
        check<atcoder::static_modint<17>>(a, b, exponent);
        check<atcoder::static_modint<12>>(a, b, exponent);
        check<atcoder::static_modint<1>>(a, b, exponent);
        Dynamic::set_mod(1 + rng() % 257); // No values survive this modulus change.
        check<Dynamic>(a, b, exponent);
    }
    ++cases_checked;
    using Other = atcoder::dynamic_modint<1>;
    Other::set_mod(19);
    Other retained = 18;
    Dynamic::set_mod(7);
    require((retained + 2).val() == 1 && Other::mod() == 19,
            "independent dynamic IDs: id1=19 retains 18; id0 changes to 7");
    success();
}
