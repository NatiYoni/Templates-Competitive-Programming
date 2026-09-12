#include "test_util.hpp"
#include "toolkit/kactl_prelude.hpp"
#include "content/data-structures/RMQ.h"

// Independent oracle: directly scan every valid nonempty subarray.
void run_case(vector<int> values) {
    ++cases_checked;
    vector<int> original = values;
    RMQ<int> table(values);
    fill(values.begin(), values.end(), 42);  // Construction must own a copy.
    string input = "original=" + show(original) + ";overwrite_input_with=42";
    for (int l = 0; l < int(original.size()); ++l) {
        int expected = INT_MAX;
        for (int r = l + 1; r <= int(original.size()); ++r) {
            expected = min(expected, original[r - 1]);
            require(table.query(l, r) == expected,
                    input + " range=" + to_string(l) + "," + to_string(r));
        }
    }
    require(table.jmp.front() == original, input + " copied storage");
}
int main() {
    run_case({});
    run_case({INT_MIN});
    run_case({INT_MAX, INT_MIN, INT_MIN, INT_MAX});
    run_case(vector<int>(33, 7));
    for (int i = 0; i < 260; ++i) {
        vector<int> values(rng() % 40);
        for (int& x : values) x = int(rng() % 31) - 15;
        run_case(values);
    }
    success();
}
