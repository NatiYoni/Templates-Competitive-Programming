#include "toolkit/matrix_exponentiation.hpp"
#include "test_util.hpp"

using toolkit::ModMatrix;
uint64_t random64() { return (uint64_t(rng()) << 32) | rng(); }
uint64_t add_mod(uint64_t a, uint64_t b, uint64_t m) {
    return a >= m - b ? a - (m - b) : a + b;
}
uint64_t product_oracle(uint64_t a, uint64_t b, uint64_t m) {
    a %= m;
    uint64_t result = 0;
    while (b) {
        if (b & 1) result = add_mod(result, a, m);
        a = add_mod(a, a, m);
        b >>= 1;
    }
    return result;
}
ModMatrix identity(size_t n, uint64_t m) {
    ModMatrix result(n, vector<uint64_t>(n));
    for (size_t i = 0; i < n; ++i) result[i][i] = 1 % m;
    return result;
}
ModMatrix product(const ModMatrix& a, const ModMatrix& b, uint64_t m) {
    size_t n = a.size();
    ModMatrix result(n, vector<uint64_t>(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < n; ++k)
                result[i][j] = add_mod(result[i][j], product_oracle(a[i][k], b[k][j], m), m);
    return result;
}
string describe(const ModMatrix& matrix) {
    string result = "[";
    for (const auto& row : matrix) result += show(row);
    return result + "]";
}
void check(const ModMatrix& a, const ModMatrix& b, uint64_t exponent, uint64_t modulus) {
    ++cases_checked;
    string input = "a=" + describe(a) + " b=" + describe(b) + " exponent="
                 + to_string(exponent) + " modulus=" + to_string(modulus);
    auto unchanged = a;
    auto expected = identity(a.size(), modulus);
    for (uint64_t i = 0; i < exponent; ++i) expected = product(expected, a, modulus);
    require(toolkit::matrix_power_mod(a, exponent, modulus) == expected, input + " power");
    require(toolkit::matrix_multiply_mod(a, b, modulus) == product(a, b, modulus), input + " multiply");
    require(a == unchanged, input + " mutation");
}

int main() {
    for (int iteration = 0; iteration < 450; ++iteration) {
        size_t n = rng() % 5;
        ModMatrix a(n, vector<uint64_t>(n)), b = a;
        for (auto& row : a) for (auto& value : row) value = random64();
        for (auto& row : b) for (auto& value : row) value = random64();
        uint64_t modulus = random64() | 1;
        if (iteration % 5 == 0) modulus = UINT64_MAX;
        if (iteration % 7 == 0) modulus = 1;
        if (iteration % 11 == 0) modulus = 2;
        check(a, b, rng() % 11, modulus);
    }
    check({{UINT64_MAX, UINT64_MAX}, {UINT64_MAX, UINT64_MAX}},
          {{UINT64_MAX, UINT64_MAX}, {UINT64_MAX, UINT64_MAX}}, 2, UINT64_MAX - 1);
    for (size_t n = 0; n <= 7; ++n) {
        ModMatrix cycle(n, vector<uint64_t>(n)), expected = cycle;
        for (size_t i = 0; i < n; ++i) {
            cycle[i][(i + 1) % n] = 1;
            expected[i][(i + UINT64_MAX % n) % n] = 1;
        }
        ++cases_checked;
        require(toolkit::matrix_power_mod(cycle, UINT64_MAX, UINT64_MAX) == expected,
                "cycle n=" + to_string(n) + " exponent=UINT64_MAX");
    }
    for (uint64_t modulus : {1ULL, 2ULL, 9223372036854775808ULL, 18446744073709551615ULL}) {
        ModMatrix diagonal{{modulus - 1, 0}, {0, 1}};
        ++cases_checked;
        auto expected = ModMatrix{{modulus - 1, 0}, {0, 1 % modulus}};
        require(toolkit::matrix_power_mod(diagonal, UINT64_MAX, modulus) == expected,
                "minus-one diagonal modulus=" + to_string(modulus));
    }
    for (auto invalid : {ModMatrix{{1, 2}}, ModMatrix{{1}, {}}, ModMatrix{{}, {}}}) {
        ++cases_checked;
        bool caught = false;
        try { toolkit::matrix_power_mod(invalid, 0, 7); }
        catch (const invalid_argument&) { caught = true; }
        require(caught, "non-square exponent zero " + describe(invalid));
        caught = false;
        try { toolkit::matrix_multiply_mod(invalid, invalid, 7); }
        catch (const invalid_argument&) { caught = true; }
        require(caught, "non-square multiplication " + describe(invalid));
    }
    for (int operation = 0; operation < 3; ++operation) {
        ++cases_checked;
        bool caught = false;
        try {
            if (operation == 0) toolkit::matrix_power_mod({}, 0, 0);
            if (operation == 1) toolkit::matrix_multiply_mod({}, {}, 0);
            if (operation == 2) toolkit::matrix_multiply_mod({}, {{1}}, 7);
        } catch (const invalid_argument&) { caught = true; }
        require(caught, "invalid matrix operation=" + to_string(operation));
    }
    success();
}
