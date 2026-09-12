#pragma once
#include "base.hpp"

namespace toolkit {
struct NimMove { size_t heap; uint64_t remaining; };

// Normal play only: remove a positive number from exactly one unsigned heap.
// Empty/all-zero positions lose. All uint64_t sizes supported, no signed input.
// O(n) time, O(1) space; input unchanged, no recursion/global state.
inline uint64_t nim_sum(const vector<uint64_t>& heaps) {
    uint64_t sum = 0;
    for (auto heap : heaps) sum ^= heap;
    return sum;
}
inline bool nim_winning(const vector<uint64_t>& heaps) { return nim_sum(heaps) != 0; }

// First winning heap by zero-based index, with its new size, or nullopt if losing.
inline optional<NimMove> nim_move(const vector<uint64_t>& heaps) {
    uint64_t sum = nim_sum(heaps);
    if (sum == 0) return nullopt;
    for (size_t i = 0; i < heaps.size(); ++i) {
        uint64_t remaining = heaps[i] ^ sum;
        if (remaining < heaps[i]) return NimMove{i, remaining};
    }
    throw logic_error("nonzero Nim xor has no winning move");
}
}
