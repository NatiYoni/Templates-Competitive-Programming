#include "toolkit/submasks.hpp"

// Input (fixed): mask=10 (binary 1010). Expected output: 10 8 2 0
int main() {
    bool first = true;
    toolkit::for_each_submask(10, [&](uint64_t submask) {
        cout << (first ? "" : " ") << submask;
        first = false;
    });
    cout << '\n';
}
