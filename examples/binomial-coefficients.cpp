#include "toolkit/binomial.hpp"

// Input (fixed): n=5, k=2 and k=6, prime modulus=1,000,000,007. Expected: 10 0
int main() {
    toolkit::Binomial<1000000007> table(5);
    cout << table.choose(5, 2).val() << ' ' << table.choose(5, 6).val() << '\n';
}
