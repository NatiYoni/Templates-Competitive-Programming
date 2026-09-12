#include "toolkit/miller_rabin.hpp"
#include "test_util.hpp"

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
bool trial_prime(uint64_t n) {
    if (n < 2) return false;
    for (uint64_t d = 2; d <= n / d; ++d) if (n % d == 0) return false;
    return true;
}
uint64_t power_oracle(uint64_t a, uint64_t e, uint64_t m) {
    uint64_t result = 1;
    while (e) {
        if (e & 1) result = product_oracle(result, a, m);
        a = product_oracle(a, a, m);
        e >>= 1;
    }
    return result;
}
void certify_prime(uint64_t n, uint64_t witness, const vector<uint64_t>& factors) {
    ++cases_checked;
    string input = "Lucas certificate n=" + to_string(n) + " witness=" + to_string(witness)
                 + " factors(n-1)=" + show(factors);
    __uint128_t product = 1;
    for (auto q : factors) {
        require(trial_prime(q), input + " nonprime certificate factor=" + to_string(q));
        product *= q;
        uint64_t value = power_oracle(witness, (n - 1) / q, n);
        require(gcd(value ? value - 1 : n - 1, n) == 1, input);
    }
    require(product == n - 1 && power_oracle(witness, n - 1, n) == 1, input);
    require(toolkit::is_prime_u64(n), input);
}

int main() {
    // Complete-factorization Lucas certificates, not another Miller-Rabin oracle.
    certify_prime(18446744073709551557ULL, 2, {2, 2, 11, 137, 547, 5594472617641ULL});
    certify_prime(2305843009213693951ULL, 37, {2, 3, 3, 5, 5, 7, 11, 13, 31, 41, 61, 151, 331, 1321});
    vector<bool> sieve(20001, true);
    sieve[0] = sieve[1] = false;
    for (size_t p = 2; p * p < sieve.size(); ++p)
        if (sieve[p]) for (size_t j = p * p; j < sieve.size(); j += p) sieve[j] = false;
    for (uint64_t n = 0; n < sieve.size(); ++n) {
        ++cases_checked;
        require(toolkit::is_prime_u64(n) == sieve[n], "sieve n=" + to_string(n));
    }
    for (int iteration = 0; iteration < 400; ++iteration) {
        uint64_t n = random64() % 100000000;
        ++cases_checked;
        require(toolkit::is_prime_u64(n) == trial_prime(n), "trial n=" + to_string(n));
    }
    vector<pair<uint64_t, bool>> edges{
        {561, false}, {1105, false}, {1729, false}, {3215031751ULL, false},
        {341550071728321ULL, false}, {3825123056546413051ULL, false},
        {18446744030759878681ULL, false}, // 4294967291^2.
        {uint64_t(1) << 63, false}, {UINT64_MAX - 1, false}, {UINT64_MAX, false},
        {4294967291ULL, true}, {2305843009213693951ULL, true},
        {18446744073709551557ULL, true}
    };
    for (auto [n, expected] : edges) {
        ++cases_checked;
        require(toolkit::is_prime_u64(n) == expected, "edge n=" + to_string(n));
    }
    for (int iteration = 0; iteration < 400; ++iteration) {
        uint64_t a = uint64_t(2) + rng() % (UINT32_MAX - 1);
        uint64_t b = uint64_t(2) + rng() % (UINT32_MAX - 1);
        uint64_t n = a * b;
        ++cases_checked;
        require(!toolkit::is_prime_u64(n), "constructed composite " + to_string(a) + "*" + to_string(b));
    }
    for (int iteration = 0; iteration < 1000; ++iteration) {
        uint64_t a = random64(), b = random64(), m = random64() | 1;
        if (iteration % 7 == 0) m = UINT64_MAX;
        if (iteration % 11 == 0) m = 1;
        uint64_t exponent = rng() % 40, expected = 1 % m;
        for (uint64_t i = 0; i < exponent; ++i) expected = product_oracle(expected, a, m);
        ++cases_checked;
        string input = "a=" + to_string(a) + " b=" + to_string(b) + " m=" + to_string(m)
                     + " exponent=" + to_string(exponent);
        require(toolkit::multiply_mod_u64(a, b, m) == product_oracle(a, b, m), input);
        require(toolkit::power_mod_u64(a, exponent, m) == expected, input);
    }
    vector<uint64_t> arithmetic_edges{0, 1, 2, uint64_t(1) << 63, UINT64_MAX - 1, UINT64_MAX};
    for (auto a : arithmetic_edges) for (auto b : arithmetic_edges)
        for (auto m : arithmetic_edges) if (m) {
            ++cases_checked;
            string input = "edge product a=" + to_string(a) + " b=" + to_string(b) + " m=" + to_string(m);
            require(toolkit::multiply_mod_u64(a, b, m) == product_oracle(a, b, m), input);
        }
    for (bool power : {false, true}) {
        ++cases_checked;
        bool caught = false;
        try {
            if (power) toolkit::power_mod_u64(0, 0, 0);
            else toolkit::multiply_mod_u64(0, 0, 0);
        } catch (const invalid_argument&) { caught = true; }
        require(caught, "zero modulus power=" + to_string(power));
    }
    success();
}
