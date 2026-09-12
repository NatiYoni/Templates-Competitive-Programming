#include "test_util.hpp"
#include "toolkit/knuth_merge_dp.hpp"

// Unoptimized independent cubic oracle: all split positions, explicit interval
// sums (no shared prefix helper and no optimized opt bounds).
__int128 cubic(const vector<long long>& a) {
    int n = int(a.size());
    vector<vector<optional<__int128>>> memo(n,vector<optional<__int128>>(n));
    auto solve = [&](auto&& self, int l, int r) -> __int128 {
        if (l >= r) return 0;
        if (memo[l][r]) return *memo[l][r];
        __int128 weight = 0;
        for (int i = l; i <= r; ++i) weight += a[i];
        optional<__int128> best;
        for (int k = l; k < r; ++k) {
            __int128 value = self(self,l,k)+self(self,k+1,r)+weight;
            if (!best || value < *best) best = value;
        }
        memo[l][r] = best;
        return *best;
    };
    return solve(solve,0,n-1);
}
void run(const vector<long long>& a) {
    ++cases_checked;
    require(toolkit::knuth_adjacent_merge_cost(a) == cubic(a), "weights=" + show(a));
}
int main() {
    run({}); run({LLONG_MAX}); run({LLONG_MAX,LLONG_MAX,LLONG_MAX});
    for (int t = 0; t < 300; ++t) {
        vector<long long> a(rng()%35);
        for (auto& x : a) x = rng()%30;
        run(a);
    }
    for (int mask = 0; mask < 256; ++mask) {
        vector<long long> a(8);
        for (int i = 0; i < 8; ++i) a[i] = (mask>>i)&1;
        run(a);
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::knuth_adjacent_merge_cost({0,-1,5}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected,"negative weight invalidates Knuth guarantee");
    ++cases_checked;
    rejected = false;
    try { toolkit::knuth_adjacent_merge_cost(vector<long long>(3001)); }
    catch (const length_error&) { rejected = true; }
    require(rejected,"dimension 3001 exceeds bound");
    success();
}
