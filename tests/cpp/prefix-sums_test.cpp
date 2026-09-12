#include "toolkit/prefix_sums.hpp"
#include "test_util.hpp"

int main() {
    int states = 1;
    for (int n = 0; n <= 5; ++n, states *= 3)
        for (int mask = 0; mask < states; ++mask) {
            vector<long long> a(n);
            int x = mask;
            for (auto& v : a) { v = array<int, 3>{-2, 0, 3}[x % 3]; x /= 3; }
            toolkit::PrefixSums p(a);
            ++cases_checked;
            require(p.size() == a.size(), show(a));
            for (int l = 0; l <= n; ++l)
                for (int r = l; r <= n; ++r) {
                    long long sum = 0;
                    for (int i = l; i < r; ++i) sum += a[i];
                    require(p.range(l, r) == sum,
                            show(a) + " l=" + to_string(l) + " r=" + to_string(r));
                }
        }
    ++cases_checked;
    toolkit::PrefixSums limits({LLONG_MAX, -LLONG_MAX, LLONG_MIN, LLONG_MAX, 1});
    require(limits.range(0, 1) == LLONG_MAX && limits.range(2, 3) == LLONG_MIN
            && limits.range(3, 4) == LLONG_MAX && limits.range(0, 5) == 0,
            "signed limits prefix construction and queries");
    for (auto a : {vector<long long>{LLONG_MAX, 1}, vector<long long>{LLONG_MIN, -1}}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::PrefixSums p(a); }
        catch (const overflow_error&) { rejected = true; }
        require(rejected, "overflowing prefix " + show(a));
    }
    for (auto a : {vector<long long>{-1, LLONG_MAX, 1},
                   vector<long long>{1, LLONG_MIN, -1}}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::PrefixSums(a).range(1, 3); }
        catch (const overflow_error&) { rejected = true; }
        require(rejected, "overflowing range [1,3) " + show(a));
    }
    for (auto range : {pair<size_t, size_t>{2, 1}, {0, 3}, {size_t(-1), 0}}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::PrefixSums({1, 2}).range(range.first, range.second); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "[1,2] invalid range=" + to_string(range.first)
                          + "," + to_string(range.second));
    }
    success();
}
