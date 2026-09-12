#include "toolkit/submasks.hpp"
#include "test_util.hpp"

int main() {
    for (uint64_t mask = 0; mask < 1024; ++mask) {
        vector<uint64_t> expected, actual;
        for (uint64_t value = 0; value <= mask; ++value)
            if ((value & mask) == value) expected.push_back(value);
        reverse(expected.begin(), expected.end());
        toolkit::for_each_submask(mask, [&](uint64_t submask) { actual.push_back(submask); });
        ++cases_checked;
        require(actual == expected, "mask=" + to_string(mask));
    }
    array<int, 4> bits{0, 31, 62, 63};
    for (int chosen = 0; chosen < 16; ++chosen) {
        uint64_t mask = 0;
        for (int i = 0; i < 4; ++i) if (chosen >> i & 1) mask |= uint64_t(1) << bits[i];
        vector<uint64_t> actual, expected;
        for (int subset = 0; subset < 16; ++subset) if ((subset & chosen) == subset) {
            uint64_t value = 0;
            for (int i = 0; i < 4; ++i) if (subset >> i & 1) value |= uint64_t(1) << bits[i];
            expected.push_back(value);
        }
        sort(expected.rbegin(), expected.rend());
        toolkit::for_each_submask(mask, [&](uint64_t submask) { actual.push_back(submask); });
        ++cases_checked;
        require(actual == expected, "high-bit mask=" + to_string(mask));
    }
    ++cases_checked;
    int calls = 0;
    bool stopped = false;
    try {
        toolkit::for_each_submask(UINT64_MAX, [&](uint64_t submask) {
            require(submask == UINT64_MAX - uint64_t(calls), "full-width mask first two values");
            if (++calls == 2) throw runtime_error("stop");
        });
    } catch (const runtime_error&) { stopped = true; }
    require(stopped && calls == 2, "callback exception stops full-width enumeration");
    success();
}
