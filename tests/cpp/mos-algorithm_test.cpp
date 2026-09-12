#include "test_util.hpp"
#include "toolkit/mo_distinct.hpp"

void run(const vector<long long>& a) {
    ++cases_checked;
    vector<pair<int,int>> ranges;
    vector<int> expected;
    string input = show(a);
    for (int l = 0; l <= int(a.size()); ++l)
        for (int r = l; r <= int(a.size()); ++r) ranges.push_back({l,r});
    shuffle(ranges.begin(), ranges.end(), rng);
    for (auto [l,r] : ranges) {
        set<long long> distinct(a.begin()+l, a.begin()+r);
        expected.push_back(int(distinct.size()));
        input += ";[" + to_string(l) + "," + to_string(r) + ")";
    }
    require(toolkit::mo_distinct(a, ranges) == expected, input);
    require(toolkit::mo_distinct(a, {}).empty(), show(a) + ";no queries");
}
int main() {
    run({}); run({LLONG_MIN, LLONG_MAX, LLONG_MIN});
    for (int t = 0; t < 260; ++t) {
        vector<long long> a(rng()%25);
        for (auto& x : a) x = int(rng()%9)-4;
        run(a);
    }
    ++cases_checked;
    for (auto range : vector<pair<int,int>>{{-1,0},{1,0},{0,2}}) {
        bool rejected = false;
        try { toolkit::mo_distinct({5}, {range}); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "invalid range=" + to_string(range.first) + "," + to_string(range.second));
    }
    success();
}
