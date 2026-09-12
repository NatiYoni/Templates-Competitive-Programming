#pragma once
#include "base.hpp"
#include "string/manacher.hpp"

namespace toolkit {
// Independently authored byte/separator adapter; Nyaan's CC0 manacher is unchanged.
// odd[i] covers [i-odd[i]+1,i+odd[i]); even[i] covers [i-even[i],i+even[i]).
// All 256 byte values are supported; the even center is the gap before i.
struct PalindromeRadii { vector<int> odd, even; };

inline PalindromeRadii palindrome_radii(const string& text) {
    if (text.size() > size_t((INT_MAX - 3) / 4))
        throw length_error("Manacher transformed index bound");
    int n = int(text.size());
    vector<int> symbols(2 * n + 1, -1);
    for (int i = 0; i < n; ++i) symbols[2 * i + 1] = (unsigned char)text[i];
    auto radii = manacher(symbols);
    PalindromeRadii result{vector<int>(n), vector<int>(n)};
    for (int i = 0; i < n; ++i) {
        result.odd[i] = radii[2 * i + 1] / 2;
        result.even[i] = radii[2 * i] / 2;
    }
    return result;
}
}
