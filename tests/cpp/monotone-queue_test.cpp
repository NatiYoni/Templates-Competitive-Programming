#include "test_util.hpp"
#include "toolkit/monotone_queue.hpp"

void run(const vector<long long>& a, size_t w) {
    ++cases_checked;
    string input = show(a) + ";w=" + to_string(w);
    auto result = toolkit::sliding_window_extrema(a, w);
    vector<long long> low, high;
    for (size_t l = 0; l + w <= a.size(); ++l) {
        auto pair = minmax_element(a.begin() + l, a.begin() + l + w);
        low.push_back(*pair.first); high.push_back(*pair.second);
    }
    require(result.minimum == low && result.maximum == high, input);
}
int main() {
    for (int t = 0; t < 280; ++t) {
        int n = 1 + int(rng() % 60);
        vector<long long> a(n);
        for (auto& x : a) x = int(rng() % 7) - 3;
        run(a, 1 + rng() % n);
    }
    for (int n = 1; n <= 12; ++n) {
        vector<long long> a(n); iota(a.begin(), a.end(), -5LL);
        run(a, 1); run(a, a.size());
        reverse(a.begin(), a.end()); run(a, 1 + n / 2);
        fill(a.begin(), a.end(), 7); run(a, 1 + n / 2);
    }
    run({LLONG_MIN, LLONG_MAX, LLONG_MIN}, 2);
    ++cases_checked;
    for (auto bad : vector<pair<vector<long long>,size_t>>{{{},0},{{},1},{{1},0},{{1},2}}) {
        bool rejected = false;
        try { toolkit::sliding_window_extrema(bad.first, bad.second); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, "invalid=" + show(bad.first) + ";w=" + to_string(bad.second));
    }
    success();
}
