#include "toolkit/pollard_rho.hpp"
#include "test_util.hpp"

uint64_t random64() { return (uint64_t(rng()) << 32) | rng(); }
vector<uint64_t> trial_factors(uint64_t n) {
    vector<uint64_t> result;
    for (uint64_t p = 2; p <= n / p; ++p)
        while (n % p == 0) { result.push_back(p); n /= p; }
    if (n > 1) result.push_back(n);
    return result;
}
bool independently_prime(uint64_t n) {
    if (n == 18446744073709551557ULL || n == 2305843009213693951ULL) return true;
    return n > 1 && trial_factors(n) == vector<uint64_t>{n};
}
void check(uint64_t n, vector<uint64_t> expected, uint64_t seed) {
    ++cases_checked;
    string input = "n=" + to_string(n) + " rho_seed=" + to_string(seed);
    sort(expected.begin(), expected.end());
    auto actual = toolkit::factor_u64(n, seed);
    require(bool(actual), input + " default budget exhausted");
    require(*actual == expected, input + " factors=" + show(*actual) + " expected=" + show(expected));
    __uint128_t product = 1;
    for (auto p : *actual) {
        require(toolkit::is_prime_u64(p) && independently_prime(p), input + " factor=" + to_string(p));
        product *= p;
    }
    require(product == n, input + " reconstruction");
}

int main() {
    for (uint64_t n = 1; n <= 600; ++n) check(n, trial_factors(n), random64());
    for (int iteration = 0; iteration < 300; ++iteration) {
        uint64_t n = 1 + random64() % 1000000000;
        check(n, trial_factors(n), random64());
    }
    vector<uint64_t> primes{4294967291ULL, 4294967279ULL, 4294967231ULL,
                             2147483647ULL, 1000000007ULL, 1000000009ULL};
    for (auto p : primes) require(independently_prime(p), "oracle prime=" + to_string(p));
    for (int iteration = 0; iteration < 24; ++iteration) {
        uint64_t p = primes[rng() % primes.size()], q = primes[rng() % primes.size()];
        check(p * q, {p, q}, random64());
    }
    check(UINT64_MAX, {3, 5, 17, 257, 641, 65537, 6700417}, random64());
    check(UINT64_MAX - 1, {2, 7, 7, 73, 127, 337, 92737, 649657}, random64());
    check(uint64_t(1) << 63, vector<uint64_t>(63, 2), random64());
    uint64_t power = 1;
    vector<uint64_t> threes;
    for (int i = 0; i < 40; ++i) { power *= 3; threes.push_back(3); }
    check(power, threes, random64());
    check(341550071728321ULL, {10670053, 32010157}, random64());
    check(3825123056546413051ULL, {149491, 747451, 34233211}, random64());
    check(18446744073709551557ULL, {18446744073709551557ULL}, random64());
    check(2305843009213693951ULL, {2305843009213693951ULL}, random64());
    for (uint64_t n : {35ULL, 70ULL, 1000036000099ULL}) {
        for (auto budget : {toolkit::RhoBudget{0, 100}, toolkit::RhoBudget{5, 0}}) {
            ++cases_checked;
            auto first = toolkit::factor_u64(n, 1729, budget);
            auto second = toolkit::factor_u64(n, 1729, budget);
            require(!first && first == second, "exhaustion n=" + to_string(n) + " attempts="
                    + to_string(budget.attempts_per_split) + " steps=" + to_string(budget.steps_per_attempt));
        }
    }
    for (uint64_t n : {1ULL, 2ULL, 3ULL, 18446744073709551557ULL}) {
        ++cases_checked;
        auto result = toolkit::factor_u64(n, 0, {0, 0});
        require(bool(result) && *result == (n == 1 ? vector<uint64_t>{} : vector<uint64_t>{n}),
                "no-work n=" + to_string(n));
    }
    ++cases_checked;
    require(!toolkit::factor_u64(1000036000099ULL, 1729, {1, 1}),
            "n=1000036000099 rho_seed=1729 attempts=1 steps=1 must exhaust");
    check(1000036000099ULL, {1000003, 1000033}, 1729);
    ++cases_checked;
    bool caught = false;
    try { toolkit::factor_u64(0, 1729); } catch (const invalid_argument&) { caught = true; }
    require(caught, "factor zero");
    success();
}
