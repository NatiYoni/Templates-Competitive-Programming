#pragma once
#include "fundamentals_arithmetic.hpp"

namespace toolkit {
struct KnapsackItem { int weight; long long value; };

// Max value with weight <=capacity, empty selection allowed. Nonnegative int
// weights/capacity checked; zero-weight items used once; signed values allowed.
// Throws overflow_error if an optimal feasible value exceeds long long.
// O(items*(capacity+1)) time, O(capacity+1) space; input unchanged, no recursion.
inline long long zero_one_knapsack(int capacity, const vector<KnapsackItem>& items) {
    if (capacity < 0) throw invalid_argument("negative capacity");
    for (auto item : items)
        if (item.weight < 0) throw invalid_argument("negative item weight");
    vector<long long> best(size_t(capacity) + 1, 0);
    for (auto item : items)
        for (int c = capacity; c >= item.weight; --c)
            best[c] = max(best[c], fundamentals_detail::checked_add(best[c - item.weight],
                                                                   item.value));
    return best[capacity];
}
}
