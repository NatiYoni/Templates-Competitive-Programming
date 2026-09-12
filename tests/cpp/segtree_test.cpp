#include "test_util.hpp"
#define TOOLKIT_EXAMPLE_NO_MAIN
#include "../../examples/segtree.cpp"

// Independent oracle: array sum, with linear left/right scans for boundaries.
void check_tree(SumTree& tree, const vector<long long>& values, const string& input) {
    int n = int(values.size());
    require(tree.all_prod() == accumulate(values.begin(), values.end(), 0LL),
            input + " all_prod");
    for (int p = 0; p < n; ++p)
        require(tree.get(p) == values[p], input + " get=" + to_string(p));
    for (int l = 0; l <= n; ++l) {
        long long expected = 0;
        for (int r = l; r <= n; ++r) {
            require(tree.prod(l, r) == expected,
                    input + " prod=" + to_string(l) + "," + to_string(r));
            if (r < n) expected += values[r];
        }
    }
    for (long long limit : {0LL, 1LL, 15LL, 1000000000000LL}) {
        auto within = [limit](long long sum) { return sum <= limit; };
        for (int start = 0; start <= n; ++start) {
            int right = start, left = start;
            long long sum = 0;
            while (right < n && sum + values[right] <= limit) sum += values[right++];
            sum = 0;
            while (left && sum + values[left - 1] <= limit) sum += values[--left];
            string query = input + " start=" + to_string(start) + " limit=" + to_string(limit);
            require(tree.max_right(start, within) == right, query + " max_right");
            require(tree.min_left(start, within) == left, query + " min_left");
        }
    }
}

void run_case(vector<long long> values) {
    ++cases_checked;
    SumTree tree(values);
    string input = "initial=" + show(values);
    check_tree(tree, values, input);
    for (int step = 0; !values.empty() && step < 25; ++step) {
        int p = int(rng() % values.size());
        long long x = rng() % 21;
        input += ";set(" + to_string(p) + "," + to_string(x) + ")";
        values[p] = x;
        tree.set(p, x);
        check_tree(tree, values, input);
    }
}

int main() {
    run_case({});
    run_case({0});
    run_case({0, 0, 0, 0, 0});
    run_case({1000000000000LL, 1000000000000LL});
    for (int i = 0; i < 260; ++i) {
        vector<long long> values(rng() % 17);
        for (auto& x : values) x = rng() % 21;
        run_case(values);
    }
    success();
}
