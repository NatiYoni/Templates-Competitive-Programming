#include "test_util.hpp"
#include "toolkit/partition_dp.hpp"

void run(const vector<long long>& a, int groups, bool negative_cost) {
    ++cases_checked;
    int n = int(a.size());
    string input = "weights=" + show(a) + ";groups=" + to_string(groups) +
                   ";negative=" + to_string(negative_cost);
    vector<long long> prefix(n+1);
    for (int i = 0; i < n; ++i) prefix[i+1] = prefix[i]+a[i];
    auto cost = [&](int l, int r) {
        require(0 <= l && l < r && r <= n, input + ";invalid callback segment");
        long long sum = prefix[r]-prefix[l];
        return sum*sum - (negative_cost ? 1000 : 0);
    };
    auto actual = toolkit::partition_dp(n, groups, cost);
    // Exhaustive combinations of cuts, independently for every prefix j.
    vector<optional<long long>> expected(n+1);
    for (int j = 0; j <= n; ++j) {
        auto enumerate = [&](auto&& self, int start, int left, long long value) -> void {
            if (!left) {
                if (start == j && (!expected[j] || value < *expected[j])) expected[j] = value;
                return;
            }
            for (int end = start+1; end <= j; ++end)
                self(self,end,left-1,value+cost(start,end));
        };
        enumerate(enumerate,0,groups,0);
    }
    require(actual == expected, input);
}
int main() {
    run({},0,false); run({},1,false);
    run({0,0,0,0},2,false); run({0,0,0,0},2,true);
    for (int t = 0; t < 300; ++t) {
        vector<long long> a(rng()%10);
        for (auto& x : a) x = rng()%6;
        run(a,int(rng()%(a.size()+3)),(rng()&1)!=0);
    }
    ++cases_checked;
    auto impossible = toolkit::partition_dp(3,4,[](int,int)->long long {
        require(false,"infeasible groups must not call cost"); return 0;
    });
    require(impossible == vector<optional<long long>>(4), "groups>n all infeasible");
    ++cases_checked;
    bool rejected = false;
    try { toolkit::partition_dp(2,2,[](int,int) { return LLONG_MAX; }); }
    catch (const overflow_error&) { rejected = true; }
    require(rejected, "LLONG_MAX + LLONG_MAX candidate");
    ++cases_checked;
    rejected = false;
    try { toolkit::partition_dp(2,2,[](int,int) { return LLONG_MIN; }); }
    catch (const overflow_error&) { rejected = true; }
    require(rejected, "LLONG_MIN + LLONG_MIN candidate");
    ++cases_checked;
    auto edge = toolkit::partition_dp(1,1,[](int,int) { return LLONG_MAX; });
    require(edge[1] && *edge[1] == LLONG_MAX && !edge[0], "max finite not INF sentinel");
    ++cases_checked;
    rejected = false;
    try { toolkit::partition_dp(-1,0,[](int,int) { return 0LL; }); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "negative dimensions");
    success();
}
