#include "toolkit/miller_rabin.hpp"

int main() {
    for (uint64_t n : {0ULL, 1ULL, 2ULL, 341550071728321ULL,
                       18446744073709551557ULL, 18446744073709551615ULL})
        cout << n << ": " << (toolkit::is_prime_u64(n) ? "prime" : "not prime") << '\n';
}
