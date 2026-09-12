#include "toolkit/kactl_prelude.hpp"
#include "content/numerical/SolveLinear.h"
#include "test_util.hpp"

using Matrix = vector<vector<long long>>;

long long determinant(const Matrix& a) {
    if (a.empty()) return 1;
    long long result = 0;
    for (int column = 0; column < int(a.size()); ++column) {
        Matrix minor(a.size() - 1);
        for (int i = 1; i < int(a.size()); ++i)
            for (int j = 0; j < int(a.size()); ++j)
                if (j != column) minor[i-1].push_back(a[i][j]);
        result += (column % 2 ? -1 : 1) * a[0][column] * determinant(minor);
    }
    return result;
}

int exact_rank(const Matrix& a, int columns) {
    int rows = int(a.size()), best = 0;
    for (unsigned r = 1; r < (1u << rows); ++r)
        for (unsigned c = 1; c < (1u << columns); ++c) {
            int k = __builtin_popcount(r);
            if (k <= best || k != __builtin_popcount(c)) continue;
            Matrix minor;
            for (int i = 0; i < rows; ++i) if (r & (1u << i)) {
                minor.push_back({});
                for (int j = 0; j < columns; ++j)
                    if (c & (1u << j)) minor.back().push_back(a[i][j]);
            }
            if (determinant(minor)) best = k;
        }
    return best;
}

void check(const Matrix& integers, const vector<long long>& rhs, int columns) {
    ++cases_checked;
    string input = "columns=" + to_string(columns) + " A=";
    for (const auto& row : integers) input += show(row);
    input += " b=" + show(rhs);
    int rank = exact_rank(integers, columns);
    Matrix augmented = integers;
    for (int i = 0; i < int(rhs.size()); ++i) augmented[i].push_back(rhs[i]);
    bool consistent = rank == exact_rank(augmented, columns + 1);
    vector<vd> a;
    for (const auto& row : integers) a.emplace_back(row.begin(), row.end());
    vd b(rhs.begin(), rhs.end()), x(columns, 42);
    int result = solveLinear(a, b, x);
    require(result == (consistent ? rank : -1), input + " rank=" + to_string(rank));
    if (!consistent) {
        require(x == vd(columns, 42), input + " failed solve must not publish a solution");
        return;
    }
    require(x.size() == size_t(columns), input + " solution shape");
    for (int i = 0; i < int(rhs.size()); ++i) {
        long double actual = 0, scale = 1 + abs(rhs[i]);
        for (int j = 0; j < columns; ++j) {
            require(isfinite(x[j]), input + " nonfinite solution");
            actual += integers[i][j] * static_cast<long double>(x[j]);
            scale += abs(integers[i][j] * static_cast<long double>(x[j]));
        }
        require(abs(actual - rhs[i]) <= 1e-9L * scale, input + " residual x=" + show(x));
    }
}

int main() {
    check({}, {}, 0);
    check({}, {}, 3);
    check({{}, {}}, {0, 0}, 0);
    check({{}}, {1}, 0);
    check({{0, 0}, {0, 0}}, {0, 0}, 2);
    check({{1, 2}, {2, 4}}, {3, 7}, 2);
    check({{1, 2}, {2, 4}}, {3, 6}, 2);
    check({{2, 0}, {0, 3}}, {1, 1}, 2); // Exact fractional solution (1/2,1/3).
    check({{0, 0, 2}, {0, 3, 0}}, {1, 2}, 3);
    for (int t = 0; t < 300; ++t) {
        int rows = rng() % 5, columns = rng() % 5;
        Matrix a(rows, vector<long long>(columns));
        vector<long long> b(rows), known(columns);
        for (auto& value : known) value = int(rng() % 7) - 3;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                a[i][j] = int(rng() % 7) - 3;
                b[i] += a[i][j] * known[j];
            }
            if (t % 2) b[i] = int(rng() % 7) - 3;
        }
        check(a, b, columns);
    }
    ++cases_checked;
    vector<vd> a{{1, 2}, {3, 4}}, original = a;
    vd b{3, 7}, original_b = b, x(2);
    require(solveLinear(a, b, x) == 2 && a != original && b != original_b,
            "A=[[1,2],[3,4]] b=[3,7] destructive elimination");
    ++cases_checked;
    a = {{eps / 2}}; b = {1}; x.assign(1, 0);
    require(solveLinear(a, b, x) == -1, "A=[[eps/2]] b=[1] absolute threshold limitation");
    success();
}
