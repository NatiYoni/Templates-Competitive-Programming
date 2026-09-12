#include "toolkit/binomial.hpp"
#include "test_util.hpp"

template<int Mod> void check(int limit) {
    toolkit::Binomial<Mod> table(limit);
    vector<vector<long long>> pascal(limit + 1, vector<long long>(limit + 1));
    for (int n = 0; n <= limit; ++n) {
        pascal[n][0] = pascal[n][n] = 1;
        for (int k = 1; k < n; ++k)
            pascal[n][k] = (pascal[n - 1][k - 1] + pascal[n - 1][k]) % Mod;
        for (int k = -1; k <= n + 1; ++k) {
            ++cases_checked;
            long long expected = k < 0 || k > n ? 0 : pascal[n][k];
            require(table.choose(n, k).val() == expected,
                    "mod=" + to_string(Mod) + " limit=" + to_string(limit)
                    + " n=" + to_string(n) + " k=" + to_string(k));
        }
    }
    for (int n : {-1, limit + 1, INT_MAX}) {
        ++cases_checked;
        bool rejected = false;
        try { table.choose(n, -1); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "mod=" + to_string(Mod) + " limit=" + to_string(limit)
                          + " invalid n=" + to_string(n) + " k=-1");
    }
    for (int k : {INT_MIN, INT_MAX}) {
        ++cases_checked;
        require(table.choose(limit, k).val() == 0,
                "mod=" + to_string(Mod) + " n=" + to_string(limit) + " k=" + to_string(k));
    }
}

int main() {
    static_assert(toolkit::binomial_prime(2) && toolkit::binomial_prime(101));
    static_assert(!toolkit::binomial_prime(1) && !toolkit::binomial_prime(9));
    check<2>(0);
    check<2>(1);
    check<7>(6);
    check<101>(100);
    check<1000000007>(50);
    for (int limit : {-1, 7, INT_MAX}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::Binomial<7> table(limit); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, "mod=7 invalid table limit=" + to_string(limit));
    }
    success();
}
