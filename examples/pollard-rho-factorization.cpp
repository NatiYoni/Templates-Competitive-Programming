#include "toolkit/pollard_rho.hpp"

int main() {
    uint64_t n = UINT64_MAX, seed = 1729;
    auto factors = toolkit::factor_u64(n, seed);
    if (!factors) {
        cerr << "Retry budget exhausted for n=" << n << " seed=" << seed << '\n';
        return 1;
    }
    for (uint64_t prime : *factors) cout << prime << ' ';
    cout << '\n'; // 3 5 17 257 641 65537 6700417
}
