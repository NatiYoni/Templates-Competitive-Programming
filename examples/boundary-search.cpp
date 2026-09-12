#include "toolkit/search.hpp"

// Input (fixed): [0,11), predicate x>=7. Expected output: 7
int main() {
    cout << toolkit::boundary_search(0, 11, [](long long x) { return x >= 7; }) << '\n';
}
