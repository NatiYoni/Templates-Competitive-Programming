#include "toolkit/knapsack.hpp"
#include "test_util.hpp"

int main() {
    for (int trial = 0; trial < 500; ++trial) {
        int n = rng() % 11, capacity = rng() % 16;
        vector<toolkit::KnapsackItem> items;
        ostringstream input;
        input << "capacity=" << capacity << " items=";
        for (int i = 0; i < n; ++i) {
            int w = rng() % 8;
            long long v = int(rng() % 31) - 10;
            items.push_back({w, v});
            input << '(' << w << ',' << v << ')';
        }
        long long best = 0;
        for (int mask = 0; mask < (1 << n); ++mask) {
            int weight = 0;
            long long value = 0;
            for (int i = 0; i < n; ++i) if (mask >> i & 1) {
                weight += items[i].weight;
                value += items[i].value;
            }
            if (weight <= capacity) best = max(best, value);
        }
        ++cases_checked;
        require(toolkit::zero_one_knapsack(capacity, items) == best, input.str());
    }
    ++cases_checked;
    require(toolkit::zero_one_knapsack(0, {{0, 3}, {0, 5}, {0, -7}, {1, 100}}) == 8,
            "capacity=0 items=(0,3),(0,5),(0,-7),(1,100)");
    ++cases_checked;
    require(toolkit::zero_one_knapsack(1, {{0, LLONG_MIN}, {1, LLONG_MAX}}) == LLONG_MAX,
            "capacity=1 items=(0,LLONG_MIN),(1,LLONG_MAX)");
    ++cases_checked;
    require(toolkit::zero_one_knapsack(0, {{INT_MAX, LLONG_MAX}}) == 0,
            "capacity=0 overweight item=(INT_MAX,LLONG_MAX)");
    ++cases_checked;
    bool rejected = false;
    try { toolkit::zero_one_knapsack(0, {{0, LLONG_MAX}, {0, 1}}); }
    catch (const overflow_error&) { rejected = true; }
    require(rejected, "capacity=0 items=(0,LLONG_MAX),(0,1)");
    ++cases_checked;
    rejected = false;
    try { toolkit::zero_one_knapsack(-1, {}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "capacity=-1 items=[]");
    ++cases_checked;
    rejected = false;
    try { toolkit::zero_one_knapsack(0, {{-1, -1}}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "capacity=0 negative-weight item=(-1,-1)");
    success();
}
