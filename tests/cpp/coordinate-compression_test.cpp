#include "toolkit/compression.hpp"
#include "test_util.hpp"

int main() {
    auto check = [](const vector<long long>& a) {
        ++cases_checked;
        string input = show(a);
        toolkit::CoordinateCompression<long long> c(a);
        set<long long> oracle(a.begin(), a.end());
        require(c.values() == vector<long long>(oracle.begin(), oracle.end()), input);
        for (auto x : a) {
            size_t rank = size_t(distance(oracle.begin(), oracle.find(x)));
            require(c.rank(x) == rank && c.values()[rank] == x, input);
        }
    };
    int states = 1;
    for (int n = 0; n <= 5; ++n, states *= 3)
        for (int mask = 0; mask < states; ++mask) {
            vector<long long> a(n);
            int x = mask;
            for (auto& v : a) { v = x % 3 - 1; x /= 3; }
            check(a);
        }
    check({LLONG_MAX, LLONG_MIN, 0, LLONG_MAX, LLONG_MIN});
    check({7, 7, 7});
    for (vector<long long> a : {vector<long long>{}, vector<long long>{-2, 0, 2}}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::CoordinateCompression<long long>(a).rank(1); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "missing rank=1 values=" + show(a));
    }
    success();
}
