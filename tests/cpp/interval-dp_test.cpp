#include "test_util.hpp"
#include "toolkit/interval_merge_dp.hpp"

// Independent oracle enumerates actual choices of adjacent piles to merge,
// rather than using interval splits or a DP table.
__int128 brute(const vector<__int128>& piles) {
    if (piles.size() < 2) return 0;
    optional<__int128> best;
    for (size_t i = 0; i+1 < piles.size(); ++i) {
        auto next = piles;
        __int128 merged = next[i] + next[i+1];
        next[i] = merged; next.erase(next.begin()+i+1);
        __int128 candidate = merged + brute(next);
        if (!best || candidate < *best) best = candidate;
    }
    return *best;
}
void run(const vector<long long>& a) {
    ++cases_checked;
    require(toolkit::adjacent_merge_cost(a) == brute(vector<__int128>(a.begin(),a.end())),
            "piles=" + show(a));
}
int main() {
    run({}); run({LLONG_MIN}); run({LLONG_MAX,LLONG_MAX,LLONG_MIN});
    for (int t = 0; t < 260; ++t) {
        vector<long long> a(rng()%8);
        for (auto& x : a) x = int(rng()%21)-10;
        run(a);
    }
    for (int mask = 0; mask < 64; ++mask) {
        vector<long long> a(6);
        for (int i = 0; i < 6; ++i) a[i] = (mask>>i)&1 ? 1 : -1;
        run(a);
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::adjacent_merge_cost(vector<long long>(501)); }
    catch (const length_error&) { rejected = true; }
    require(rejected, "dimension 501 exceeds cubic bound");
    success();
}
