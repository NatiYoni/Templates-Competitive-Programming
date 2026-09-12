#include "toolkit/compression.hpp"

// Input (fixed): [50,-4,50,9]. Expected output: 2 0 2 1
int main() {
    vector<long long> values{50, -4, 50, 9};
    toolkit::CoordinateCompression<long long> compressed(values);
    for (size_t i = 0; i < values.size(); ++i)
        cout << (i ? " " : "") << compressed.rank(values[i]);
    cout << '\n';
}
