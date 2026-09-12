#include "test_util.hpp"
#include "toolkit/parallel_binary_search.hpp"

void run(const vector<int>& updates, const vector<int>& thresholds) {
    ++cases_checked;
    int n = int(updates.size()), prefix = -1, sum = 12345, rounds = 0;
    string input = "updates=" + show(updates) + ";thresholds=" + show(thresholds);
    auto answer = toolkit::parallel_binary_search(n, int(thresholds.size()), [&] {
        require(prefix == -1 || prefix == n, input + ";incomplete previous round");
        prefix = 0; sum = 0; ++rounds;
    }, [&](int i) {
        require(i == prefix, input + ";apply index=" + to_string(i));
        sum += updates[i]; ++prefix;
    }, [&](int q) {
        require(sum == accumulate(updates.begin(), updates.begin()+prefix, 0),
                input + ";bad reset/prefix");
        return sum >= thresholds[q];
    });
    vector<int> expected;
    for (int threshold : thresholds) {
        int running = 0, found = n + 1;
        for (int t = 0; t <= n; ++t) {
            if (running >= threshold) { found = t; break; }
            if (t < n) running += updates[t];
        }
        expected.push_back(found);
    }
    require(answer == expected, input);
    require(thresholds.empty() ? rounds == 0 : rounds > 0 && prefix == n, input + ";lifecycle");
}
int main() {
    run({}, {}); run({}, {-1,0,1}); run({0,0,0},{0,1});
    for (int t = 0; t < 280; ++t) {
        vector<int> updates(rng()%30), thresholds{-1,0,10000};
        for (auto& x : updates) x = int(rng()%6);
        for (int q = 0; q < 30; ++q) thresholds.push_back(int(rng()%100));
        run(updates, thresholds);
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::parallel_binary_search(-1, 1, []{}, [](int){}, [](int){return false;}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "negative update count");
    success();
}
