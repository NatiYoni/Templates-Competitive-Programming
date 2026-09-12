#include "toolkit/matrix_exponentiation.hpp"

int main() {
    toolkit::ModMatrix fibonacci{{1, 1}, {1, 0}};
    auto tenth = toolkit::matrix_power_mod(fibonacci, 10, 1000000007);
    cout << tenth[0][1] << '\n'; // F(10)=55.
    auto identity = toolkit::matrix_power_mod(fibonacci, 0, UINT64_MAX);
    for (const auto& row : identity) {
        for (auto value : row) cout << value << ' ';
        cout << '\n';
    }
}
