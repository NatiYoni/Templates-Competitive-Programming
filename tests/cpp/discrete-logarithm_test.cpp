#include "toolkit/discrete_logarithm.hpp"
#include "test_util.hpp"

uint64_t random64() { return (uint64_t(rng()) << 32) | rng(); }
vector<optional<uint64_t>> orbit(uint64_t a, uint64_t m) {
    vector<optional<uint64_t>> answers(m);
    uint64_t value = 1 % m, exponent = 0;
    while (!answers[value]) {
        answers[value] = exponent++;
        value = value * (a % m) % m; // Both factors <2^32, independently safe in uint64_t.
    }
    return answers;
}
void check(uint64_t a, uint64_t b, uint64_t m, optional<uint64_t> expected) {
    ++cases_checked;
    string input = "a=" + to_string(a) + " b=" + to_string(b) + " m=" + to_string(m);
    auto actual = toolkit::discrete_log(a, b, m);
    require(actual == expected, input + " expected=" + (expected ? to_string(*expected) : "none")
            + " actual=" + (actual ? to_string(*actual) : "none"));
}

int main() {
    for (uint64_t m = 1; m <= 60; ++m)
        for (uint64_t a = 0; a < m; ++a) {
            auto expected = orbit(a, m);
            for (uint64_t b = 0; b < m; ++b) check(a, b, m, expected[b]);
        }
    for (int iteration = 0; iteration < 400; ++iteration) {
        uint64_t m = 1 + rng() % 1000, a = random64(), b = random64();
        check(a, b, m, orbit(a, m)[b % m]);
    }
    for (uint64_t m : {1ULL, 2ULL, 2147483648ULL, 4294967291ULL, 4294967295ULL}) {
        check(0, 1, m, 0);
        check(0, 0, m, m == 1 ? 0 : 1);
        check(UINT64_MAX, UINT64_MAX, m, UINT64_MAX % m == 1 % m ? 0 : 1);
        if (m > 2) {
            check(1, 2, m, nullopt);
            check(0, 2, m, nullopt);
            check(m - 1, m - 1, m, 1);
            check(m - 1, 2, m, nullopt);
        }
    }
    for (uint64_t exponent = 0; exponent <= 31; ++exponent)
        check(2, exponent == 31 ? 0 : uint64_t(1) << exponent, uint64_t(1) << 31, exponent);
    for (int iteration = 0; iteration < 12; ++iteration) {
        uint64_t m = UINT32_MAX - uint64_t(iteration), a = 2 + rng() % 100000;
        uint64_t steps = 1 + rng() % 150, value = 1, answer = 0;
        vector<uint64_t> powers{value};
        for (uint64_t i = 0; i < steps; ++i) {
            value = value * a % m;
            powers.push_back(value);
        }
        while (powers[answer] != value) ++answer;
        check(a, value, m, answer);
    }
    for (uint64_t modulus : {0ULL, 4294967296ULL, 18446744073709551615ULL}) {
        ++cases_checked;
        bool caught = false;
        try { toolkit::discrete_log(2, 4, modulus); }
        catch (const invalid_argument&) { caught = true; }
        require(caught, "invalid modulus=" + to_string(modulus));
    }
    success();
}
