#include "test_util.hpp"
#include "toolkit/dynamic_li_chao.hpp"

void run(long long lo, long long hi, bool wide) {
    ++cases_checked;
    toolkit::DynamicLiChaoMin tree(lo, hi);
    vector<pair<long long,long long>> lines;
    string input = "domain=" + to_string(lo) + "," + to_string(hi);
    require(!tree.query(lo) && !tree.query(hi), input + ";empty");
    for (int step = 0; step < 40; ++step) {
        long long a = int(rng()%61)-30, b = int(rng()%101)-50;
        if (wide && step < 4) {
            a = step%2 ? LLONG_MIN : LLONG_MAX;
            b = step<2 ? LLONG_MAX : LLONG_MIN;
        }
        tree.add(a,b); lines.push_back({a,b});
        input += ";line=" + to_string(a) + "," + to_string(b);
        vector<long long> points{lo,hi,(long long)((__int128)lo+((__int128)hi-lo)/2)};
        if (!wide) for (long long x = lo; x <= hi; ++x) points.push_back(x);
        for (long long x : points) {
            __int128 expected = (__int128)lines[0].first*x+lines[0].second;
            for (auto [slope,intercept] : lines)
                expected = min(expected, (__int128)slope*x+intercept);
            require(tree.query(x) == optional<__int128>(expected), input + ";x=" + to_string(x));
        }
    }
}
int main() {
    for (int t = 0; t < 260; ++t) {
        long long lo = int(rng()%21)-10;
        run(lo, lo+rng()%25, false);
    }
    run(LLONG_MIN,LLONG_MAX,true);
    run(LLONG_MIN,LLONG_MIN,true);
    run(LLONG_MAX,LLONG_MAX,true);
    ++cases_checked;
    toolkit::DynamicLiChaoMin tree(-2,2,1);
    tree.add(1,0);
    bool rejected = false;
    try { tree.add(0,0); } catch (const length_error&) { rejected = true; }
    require(rejected && tree.query(2) == optional<__int128>(2), "node budget preserves lines");
    ++cases_checked;
    rejected = false;
    try { tree.query(3); } catch (const out_of_range&) { rejected = true; }
    require(rejected, "out-of-domain x=3 in [-2,2]");
    ++cases_checked;
    rejected = false;
    try { toolkit::DynamicLiChaoMin bad(1,0); } catch (const invalid_argument&) { rejected = true; }
    require(rejected, "reversed domain");
    success();
}
