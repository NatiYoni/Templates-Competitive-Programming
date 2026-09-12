#pragma once
#include "base.hpp"

// Independently authored dynamic square matrices over Z/modulus Z.
namespace toolkit {
using ModMatrix = vector<vector<uint64_t>>;

namespace matrix_detail {
inline void square(const ModMatrix& matrix) {
    for (const auto& row : matrix)
        if (row.size() != matrix.size()) throw invalid_argument("matrix is not square");
}
inline ModMatrix multiply(const ModMatrix& a, const ModMatrix& b, uint64_t modulus) {
    size_t n = a.size();
    ModMatrix result(n, vector<uint64_t>(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t k = 0; k < n; ++k)
            for (size_t j = 0; j < n; ++j)
                result[i][j] = (__uint128_t(a[i][k]) * b[k][j] + result[i][j]) % modulus;
    return result;
}
}

// Matrices must be square, equal-sized, and modulus positive. Inputs can be
// unreduced uint64_t; one product plus a reduced accumulator fits __uint128_t.
inline ModMatrix matrix_multiply_mod(const ModMatrix& a, const ModMatrix& b,
                                     uint64_t modulus) {
    if (!modulus) throw invalid_argument("zero modulus");
    matrix_detail::square(a);
    matrix_detail::square(b);
    if (a.size() != b.size()) throw invalid_argument("matrix dimension mismatch");
    return matrix_detail::multiply(a, b, modulus);
}

// Exponent 0 yields identity modulo modulus (all zeros when modulus=1).
// The empty 0x0 matrix is supported. No global modulus or integer-ring overflow.
inline ModMatrix matrix_power_mod(ModMatrix matrix, uint64_t exponent, uint64_t modulus) {
    if (!modulus) throw invalid_argument("zero modulus");
    matrix_detail::square(matrix);
    size_t n = matrix.size();
    ModMatrix result(n, vector<uint64_t>(n));
    for (size_t i = 0; i < n; ++i) {
        result[i][i] = 1 % modulus;
        for (auto& value : matrix[i]) value %= modulus;
    }
    while (exponent) {
        if (exponent & 1) result = matrix_detail::multiply(result, matrix, modulus);
        exponent >>= 1;
        if (exponent) matrix = matrix_detail::multiply(matrix, matrix, modulus);
    }
    return result;
}
}
