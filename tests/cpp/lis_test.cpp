#include "toolkit/kactl_prelude.hpp"
#include "content/various/LIS.h"
#include "test_util.hpp"

void check(const vi& values) {
    ++cases_checked;
    int best = 0, n = int(values.size());
    for (unsigned mask = 0; mask < (1u << n); ++mask) {
        int length = 0, last = 0;
        bool valid = true;
        for (int i = 0; i < n; ++i) if (mask & (1u << i)) {
            if (length && last >= values[i]) valid = false;
            last = values[i];
            ++length;
        }
        if (valid) best = max(best, length);
    }
    auto indices = lis(values);
    string input = "values=" + show(values) + " indices=" + show(indices);
    require(int(indices.size()) == best, input + " optimal length=" + to_string(best));
    for (int i = 0; i < int(indices.size()); ++i) {
        require(0 <= indices[i] && indices[i] < n, input + " index bounds");
        if (i) require(indices[i-1] < indices[i] && values[indices[i-1]] < values[indices[i]],
                       input + " strict reconstruction");
    }
}

int main() {
    for (vi v : {vi{}, vi{4}, vi{2, 2, 2}, vi{5, 4, 3, 2, 1},
                 vi{1, 2, 3, 4, 5}, vi{INT_MIN, 0, INT_MAX, INT_MIN}})
        check(v);
    for (int t = 0; t < 300; ++t) {
        vi values(rng() % 15);
        for (int& x : values) x = int(rng() % 11) - 5;
        check(values);
    }
    success();
}
