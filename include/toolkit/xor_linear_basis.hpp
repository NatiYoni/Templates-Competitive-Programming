#pragma once
#include "base.hpp"
#include "math/matrix/binary-basis.hpp"

// Authored wrapper around unchanged Luzhiled BinaryBasis (Unlicense).
// Only add/check/size are exposed: upstream get_kth's signed shifts are not
// suitable for rank 63/64. Values and maximization are unsigned, including bit63.
namespace toolkit {
class XorBasis {
    BinaryBasis<uint64_t> data;
public:
    bool insert(uint64_t value) { return data.add(value); }
    bool contains(uint64_t value) const { return data.check(value); }
    size_t rank() const { return data.size(); }

    // Maximum of (offset XOR subset XOR), including the empty subset.
    uint64_t maximum_xor(uint64_t offset = 0) const {
        auto rows = data.basis;
        sort(rows.begin(), rows.end(), greater<uint64_t>());
        for (auto row : rows) offset = max(offset, offset ^ row);
        return offset;
    }
};
}
