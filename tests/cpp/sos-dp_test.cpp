#include "test_util.hpp"
#include "toolkit/subset_transforms.hpp"

void run(const vector<long long>& a) {
    ++cases_checked;
    int n = int(a.size());
    vector<long long> zeta(n), mobius(n);
    // Independent all-pairs set-containment oracle, not the bitwise transform.
    for (int mask = 0; mask < n; ++mask)
        for (int sub = 0; sub < n; ++sub) if ((mask & sub) == sub) {
            zeta[mask] += a[sub];
            int removed = __builtin_popcount(unsigned(mask^sub));
            mobius[mask] += removed%2 ? -a[sub] : a[sub];
        }
    auto actual = toolkit::subset_zeta_sum(a);
    require(actual == zeta, "zeta input=" + show(a));
    require(toolkit::subset_mobius_sum(a) == mobius, "mobius input=" + show(a));
    require(toolkit::subset_mobius_sum(actual) == a, "roundtrip input=" + show(a));
}
int main() {
    for (int t = 0; t < 280; ++t) {
        vector<long long> a(size_t(1)<<(rng()%8));
        for (auto& x : a) x = int(rng()%101)-50;
        run(a);
    }
    run({0}); run({42});
    ++cases_checked;
    require(toolkit::subset_zeta_sum({LLONG_MIN}) == vector<long long>{LLONG_MIN} &&
            toolkit::subset_mobius_sum({LLONG_MAX}) == vector<long long>{LLONG_MAX},
            "singleton signed limits");
    ++cases_checked;
    for (int n : {0,3,5,6}) {
        bool rejected = false;
        try { toolkit::subset_zeta_sum(vector<long long>(n)); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected,"invalid length=" + to_string(n));
    }
    ++cases_checked;
    vector<vector<long long>> overflow{{LLONG_MAX,1},{LLONG_MIN,-1}};
    for (const auto& a : overflow) {
        bool rejected = false;
        try { toolkit::subset_zeta_sum(a); }
        catch (const overflow_error&) { rejected = true; }
        require(rejected,"zeta overflow input=" + show(a));
    }
    ++cases_checked;
    for (const auto& a : vector<vector<long long>>{{-1,LLONG_MAX},{1,LLONG_MIN}}) {
        bool rejected = false;
        try { toolkit::subset_mobius_sum(a); }
        catch (const overflow_error&) { rejected = true; }
        require(rejected,"mobius overflow input=" + show(a));
    }
    success();
}
