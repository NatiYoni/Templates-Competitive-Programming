#pragma once
#include "base.hpp"

namespace toolkit {
// Repository-original polynomial hashing, not adapted from upstream/std::hash.
// This is a randomized filter, NEVER a proof of string equality.
class RollingHashContext {
    const uint64_t base_;
    static uint64_t choose_base(uint64_t seed) {
        mt19937_64 generator(seed);
        return uniform_int_distribution<uint64_t>(257, modulus - 2)(generator);
    }
public:
    static constexpr uint64_t modulus = (uint64_t(1) << 61) - 1;
    explicit RollingHashContext(uint64_t seed) : base_(choose_base(seed)) {}
    uint64_t base() const { return base_; }
};

class RollingHash {
    shared_ptr<const RollingHashContext> context_;
    vector<uint64_t> prefix_, power_;
    static uint64_t multiply(uint64_t a, uint64_t b) {
        return __uint128_t(a) * b % RollingHashContext::modulus;
    }
public:
    struct Digest {
        uint64_t value;
        size_t length;
        bool operator==(const Digest& other) const {
            return length == other.length && value == other.value;
        }
    };
    RollingHash(const string& text, shared_ptr<const RollingHashContext> context)
        : context_(std::move(context)) {
        if (!context_) throw invalid_argument("rolling hash null context");
        if (text.size() >= prefix_.max_size())
            throw length_error("rolling hash prefix size");
        prefix_.resize(text.size() + 1);
        power_.resize(text.size() + 1, 1);
        const uint64_t mod = RollingHashContext::modulus, base = context_->base();
        for (size_t i = 0; i < text.size(); ++i) {
            prefix_[i + 1] = (multiply(prefix_[i], base) + (unsigned char)text[i] + 1) % mod;
            power_[i + 1] = multiply(power_[i], base);
        }
    }
    size_t size() const { return prefix_.size() - 1; }
    Digest slice(size_t left, size_t right) const {
        if (left > right || right > size()) throw out_of_range("rolling hash slice");
        uint64_t removed = multiply(prefix_[left], power_[right - left]);
        return {(prefix_[right] + RollingHashContext::modulus - removed) %
                    RollingHashContext::modulus, right - left};
    }
    bool possibly_equal(size_t left, size_t right, const RollingHash& other,
                        size_t other_left, size_t other_right) const {
        if (context_.get() != other.context_.get())
            throw invalid_argument("rolling hash comparison requires shared context");
        return slice(left, right) == other.slice(other_left, other_right);
    }
};
}
