#pragma once
#include "base.hpp"
#include <atcoder/modint.hpp>

namespace toolkit {
constexpr bool binomial_prime(int n) {
    if (n < 2) return false;
    for (int d = 2; d <= n / d; ++d) if (n % d == 0) return false;
    return true;
}

// Prime int modulus checked at compile time. Precompute 0<=limit<Mod; bad
// limits throw invalid_argument. choose requires 0<=n<=limit (out_of_range),
// and returns zero for k<0 or k>n. O(limit+log Mod) build, O(1) query/stack,
// O(limit) storage; immutable after build, no dynamic modulus/global state.
template<int Mod> struct Binomial {
    static_assert(binomial_prime(Mod), "Binomial requires a prime modulus");
    using mint = atcoder::static_modint<Mod>;
    explicit Binomial(int limit) {
        if (limit < 0 || limit >= Mod) throw invalid_argument("binomial table limit");
        factorial_.assign(size_t(limit) + 1, 1);
        inverse_factorial_.resize(size_t(limit) + 1);
        for (int n = 1; n <= limit; ++n) factorial_[n] = factorial_[n - 1] * n;
        inverse_factorial_[limit] = factorial_[limit].inv();
        for (int n = limit; n > 0; --n)
            inverse_factorial_[n - 1] = inverse_factorial_[n] * n;
    }
    mint choose(int n, int k) const {
        if (n < 0 || size_t(n) >= factorial_.size()) throw out_of_range("binomial n");
        if (k < 0 || k > n) return 0;
        return factorial_[n] * inverse_factorial_[k] * inverse_factorial_[n - k];
    }
private:
    vector<mint> factorial_, inverse_factorial_;
};
}
