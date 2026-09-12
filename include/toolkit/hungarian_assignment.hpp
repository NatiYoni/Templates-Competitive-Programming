#pragma once
#include "base.hpp"
#include <graph/flow/hungarian.hpp>

namespace toolkit {
// Original validation/index adapter; unmodified Luzhiled Hungarian (Unlicense).
// Full signed long long costs; wide potentials. Throws overflow_error if the
// optimum cannot be returned as long long. All rows assigned to distinct columns.
struct Assignment { long long cost; vector<int> column; };
inline Assignment hungarian_assignment(const vector<vector<long long>>& cost) {
    if (cost.empty()) return {0, {}};
    if (cost.size() >= size_t(INT_MAX) || cost[0].size() >= size_t(INT_MAX))
        throw invalid_argument("assignment dimensions");
    int n = int(cost.size()), m = int(cost[0].size());
    if (n > m) throw invalid_argument("assignment requires rows <= columns");
    for (const auto& row : cost)
        if (row.size() != size_t(m)) throw invalid_argument("ragged assignment matrix");
    Matrix<__int128> matrix(n + 1, m + 1);
    for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j)
        matrix[i + 1][j + 1] = cost[i][j];
    auto [optimum, rows] = ::hungarian(matrix);
    if (optimum < LLONG_MIN || optimum > LLONG_MAX)
        throw overflow_error("assignment optimum");
    Assignment result{(long long)optimum, vector<int>(n)};
    for (int j = 1; j <= m; ++j) if (rows[j]) result.column[rows[j] - 1] = j - 1;
    return result;
}
}
