#include "toolkit/parallel_binary_search.hpp"

// Prefix sums [0,2,5,6], thresholds [0,4,8]. Expected: 0 2 4
// 0 is initially true; 4 == updates+1 is never true, NOT a real prefix.
int main() {
    vector<int> updates{2, 3, 1}, threshold{0, 4, 8};
    int sum = 0;
    auto result = toolkit::parallel_binary_search(3, 3, [&] { sum = 0; },
        [&](int i) { sum += updates[i]; }, [&](int q) { return sum >= threshold[q]; });
    for (size_t i = 0; i < result.size(); ++i) cout << (i ? " " : "") << result[i];
    cout << '\n';
}
