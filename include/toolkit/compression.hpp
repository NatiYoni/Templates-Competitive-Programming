#pragma once
#include "base.hpp"

namespace toolkit {
// Sort/deduplicate a copy; ranks are zero-based size_t, absent keys throw.
// T's < must be a stable strict weak ordering (no NaN floating-point keys).
// Build O(n log n), rank O(log n), O(n) storage; no global state.
// Local logic is iterative; standard sort may use O(log n) stack.
template<class T> struct CoordinateCompression {
    explicit CoordinateCompression(vector<T> values) : values_(move(values)) {
        sort(values_.begin(), values_.end());
        values_.erase(unique(values_.begin(), values_.end(),
                            [](const T& a, const T& b) { return !(a < b) && !(b < a); }),
                      values_.end());
    }
    const vector<T>& values() const { return values_; }
    size_t rank(const T& value) const {
        auto it = lower_bound(values_.begin(), values_.end(), value);
        if (it == values_.end() || value < *it) throw out_of_range("unregistered coordinate");
        return size_t(it - values_.begin());
    }
private:
    vector<T> values_;
};
}
