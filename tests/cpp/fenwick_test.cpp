#include "test_util.hpp"
#include <atcoder/fenwicktree.hpp>

// Independent oracle: mutable array and direct signed summation.
void check_sums(atcoder::fenwick_tree<long long>& tree,
                const vector<long long>& values, const string& input) {
    int n = int(values.size());
    for (int l = 0; l <= n; ++l) {
        long long expected = 0;
        for (int r = l; r <= n; ++r) {
            require(tree.sum(l, r) == expected,
                    input + " range=" + to_string(l) + "," + to_string(r));
            if (r < n) expected += values[r];
        }
    }
}

void run_case(vector<long long> values, int updates) {
    ++cases_checked;
    atcoder::fenwick_tree<long long> tree(int(values.size()));
    string input = "initial=" + show(values);
    for (int i = 0; i < int(values.size()); ++i) tree.add(i, values[i]);
    check_sums(tree, values, input);
    for (int step = 0; !values.empty() && step < updates; ++step) {
        int p = int(rng() % values.size());
        long long delta = int(rng() % 2001) - 1000;
        input += ";add(" + to_string(p) + "," + to_string(delta) + ")";
        values[p] += delta;
        tree.add(p, delta);
        check_sums(tree, values, input);
    }
}

int main() {
    run_case({}, 0);
    run_case({0}, 20);
    run_case({LLONG_MAX / 4, LLONG_MAX / 4, -LLONG_MAX / 4, -LLONG_MAX / 4}, 0);
    run_case({LLONG_MIN}, 0);
    for (int i = 0; i < 260; ++i) {
        vector<long long> values(rng() % 18);
        for (auto& x : values) x = int(rng() % 201) - 100;
        run_case(values, 35);
    }
    success();
}
