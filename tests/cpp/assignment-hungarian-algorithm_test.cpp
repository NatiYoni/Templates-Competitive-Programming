#include "toolkit/hungarian_assignment.hpp"
#include "test_util.hpp"

string describe(const vector<vector<long long>>& cost) {
    string s = "cost=";
    for (const auto& row : cost) s += show(row);
    return s;
}
void check(const vector<vector<long long>>& cost) {
    ++cases_checked;
    string input = describe(cost);
    int n = cost.size(), m = n ? cost[0].size() : 0;
    optional<__int128> best;
    auto enumerate = [&](auto&& self, int row, int used, __int128 sum) -> void {
        if (row == n) { if (!best || sum < *best) best = sum; return; }
        for (int col = 0; col < m; ++col) if (!(used >> col & 1))
            self(self, row + 1, used | (1 << col), sum + cost[row][col]);
    };
    enumerate(enumerate, 0, 0, 0);
    if (*best < LLONG_MIN || *best > LLONG_MAX) {
        bool rejected = false;
        try { toolkit::hungarian_assignment(cost); } catch (const overflow_error&) { rejected = true; }
        require(rejected, input + " optimum outside long long");
        return;
    }
    auto result = toolkit::hungarian_assignment(cost);
    require(__int128(result.cost) == *best && result.column.size() == size_t(n), input);
    vector<bool> used(m);
    __int128 sum = 0;
    for (int i = 0; i < n; ++i) {
        int j = result.column[i];
        require(j >= 0 && j < m && !used[j], input + " matching");
        used[j] = true; sum += cost[i][j];
    }
    require(sum == *best, input + " recovered cost");
}
int main() {
    for (int t = 0; t < 700; ++t) {
        int n = rng() % 5, m = n + rng() % (7 - n);
        vector<vector<long long>> cost(n, vector<long long>(m));
        for (auto& row : cost) for (auto& c : row) c = int(rng() % 41) - 20;
        check(cost);
    }
    check({{LLONG_MIN}}); check({{LLONG_MAX}});
    check({{LLONG_MIN, LLONG_MAX}, {LLONG_MIN, LLONG_MAX}});
    check({{LLONG_MAX, LLONG_MAX}, {LLONG_MAX, LLONG_MAX}});
    check({{LLONG_MIN, LLONG_MIN}, {LLONG_MIN, LLONG_MIN}});
    check({{0, 0, 0}, {0, 0, 0}});
    check({{-8, -1, -5, -4}});
    for (auto cost : vector<vector<vector<long long>>>{{{}}, {{1}, {2}}, {{1, 2}, {3}}}) {
        ++cases_checked; bool rejected = false;
        try { toolkit::hungarian_assignment(cost); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(cost));
    }
    success();
}
