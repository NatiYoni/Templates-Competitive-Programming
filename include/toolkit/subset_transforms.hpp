#pragma once
#include "fundamentals_arithmetic.hpp"
#include "set-function/zeta-mobius-transform.hpp"

namespace toolkit {
namespace subset_transform_detail {
struct Checked {
    long long value;
    Checked& operator+=(Checked b) {
        value = fundamentals_detail::checked_add(value, b.value);
        return *this;
    }
    Checked& operator-=(Checked b) {
        value = fundamentals_detail::checked_subtract(value, b.value);
        return *this;
    }
};
inline vector<long long> transform(const vector<long long>& input, bool inverse) {
    size_t n = input.size();
    if (n == 0 || n > (size_t(1) << 22) || (n & (n - 1)))
        throw invalid_argument("subset transform requires 2^k entries, 0<=k<=22");
    vector<Checked> work;
    work.reserve(n);
    for (long long x : input) work.push_back({x});
    if (inverse) subset_mobius_transform(work);
    else subset_zeta_transform(work);
    vector<long long> answer;
    answer.reserve(n);
    for (auto x : work) answer.push_back(x.value);
    return answer;
}
}
// Checked integration of unchanged Nyaan subset transforms (CC0-1.0).
// zeta[mask]=sum_{s subset mask}input[s]; Mobius is its signed inverse.
// Nonempty power-of-two size<=2^22; any intermediate long long overflow throws,
// even if later cancellation would fit. Input unchanged including on failure.
// O(k*2^k) time, O(2^k) additional memory, no recursion or global state.
inline vector<long long> subset_zeta_sum(const vector<long long>& input) {
    return subset_transform_detail::transform(input, false);
}
inline vector<long long> subset_mobius_sum(const vector<long long>& input) {
    return subset_transform_detail::transform(input, true);
}
}
