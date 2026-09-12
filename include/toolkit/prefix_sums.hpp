#pragma once
#include "fundamentals_arithmetic.hpp"

namespace toolkit {
// Immutable sums, zero-based [l,r); empty input/ranges are valid.
// Build O(n), query O(1), storage O(n). Every prefix and requested range must
// fit long long, else overflow_error; bad ranges throw out_of_range.
struct PrefixSums {
    explicit PrefixSums(const vector<long long>& values) : sums_(1, 0) {
        for (auto value : values)
            sums_.push_back(fundamentals_detail::checked_add(sums_.back(), value));
    }
    size_t size() const { return sums_.size() - 1; }
    long long range(size_t l, size_t r) const {
        if (l > r || r >= sums_.size()) throw out_of_range("prefix-sum range");
        return fundamentals_detail::checked_subtract(sums_[r], sums_[l]);
    }
private:
    vector<long long> sums_;
};
}
