#include "toolkit/search.hpp"
#include "test_util.hpp"

int main() {
    auto check = [](long long lo, long long hi, long long boundary) {
        ++cases_checked;
        string input = "lo=" + to_string(lo) + " hi=" + to_string(hi)
                     + " boundary=" + to_string(boundary);
        int calls = 0;
        auto result = toolkit::boundary_search(lo, hi, [&](long long x) {
            require(lo <= x && x < hi, input + " predicate argument=" + to_string(x));
            ++calls;
            return x >= boundary;
        });
        require(result == boundary && calls <= 64, input);
    };
    for (long long lo = -10; lo <= 10; ++lo)
        for (long long hi = lo; hi <= 10; ++hi)
            for (long long boundary = lo; boundary <= hi; ++boundary)
                check(lo, hi, boundary);
    for (long long boundary : {LLONG_MIN, LLONG_MIN + 1, -1LL, 0LL, 1LL,
                               LLONG_MAX - 1, LLONG_MAX})
        check(LLONG_MIN, LLONG_MAX, boundary);
    check(LLONG_MIN, LLONG_MIN, LLONG_MIN);
    check(LLONG_MAX, LLONG_MAX, LLONG_MAX);
    ++cases_checked;
    bool rejected = false;
    try { toolkit::boundary_search(1, 0, [](long long) { return true; }); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "inverted domain [1,0)");
    success();
}
