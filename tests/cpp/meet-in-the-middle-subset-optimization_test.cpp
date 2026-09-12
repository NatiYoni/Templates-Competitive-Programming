#include "test_util.hpp"
#include "toolkit/meet_in_middle_subset.hpp"

void run(const vector<long long>& a, long long limit) {
    ++cases_checked;
    optional<toolkit::SubsetOptimum> expected;
    for (unsigned long long mask = 0; mask < (1ULL<<a.size()); ++mask) {
        __int128 sum = 0;
        for (size_t i = 0; i < a.size(); ++i) if ((mask>>i)&1) sum += a[i];
        if (sum <= limit && (!expected || sum > expected->sum))
            expected = toolkit::SubsetOptimum{sum,mask};
    }
    auto actual = toolkit::meet_in_middle_subset(a,limit);
    string input = "values=" + show(a) + ";limit=" + to_string(limit);
    require(bool(actual) == bool(expected),input + ";feasibility");
    if (actual) {
        require(actual->sum == expected->sum && actual->mask == expected->mask,input + ";optimum/tie");
        __int128 sum = 0;
        for (size_t i = 0; i < a.size(); ++i) if ((actual->mask>>i)&1) sum += a[i];
        require(sum == actual->sum && sum <= limit && (actual->mask>>a.size()) == 0,
                input + ";reconstruction");
    }
}
int main() {
    run({},0); run({},-1); run({5,6},-1);
    run({LLONG_MIN,LLONG_MIN,LLONG_MAX},LLONG_MIN);
    run({LLONG_MAX,LLONG_MAX,LLONG_MIN},LLONG_MAX);
    run({-5,-5,10,10},-11); run({0,0,0},0);
    for (int t = 0; t < 300; ++t) {
        vector<long long> a(rng()%16);
        for (auto& x : a) x = int(rng()%41)-20;
        run(a,int(rng()%101)-50);
    }
    ++cases_checked;
    auto maximum = toolkit::meet_in_middle_subset(vector<long long>(40,0),0);
    require(maximum && maximum->sum == 0 && maximum->mask == 0,"40 zero items tie mask");
    ++cases_checked;
    bool rejected = false;
    try { toolkit::meet_in_middle_subset(vector<long long>(41),0); }
    catch (const length_error&) { rejected = true; }
    require(rejected,"41 items rejected");
    success();
}
