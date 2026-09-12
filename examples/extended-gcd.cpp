#include "toolkit/extended_gcd.hpp"

string decimal(__int128_t value) {
    bool negative = value < 0;
    if (negative) value = -value;
    string result;
    do { result += char('0' + value % 10); value /= 10; } while (value);
    if (negative) result += '-';
    reverse(result.begin(), result.end());
    return result;
}

int main() {
    int64_t a = INT64_MIN, b = 15;
    auto result = toolkit::extended_gcd(a, b);
    cout << a << " * " << decimal(result.x) << " + " << b << " * "
         << decimal(result.y) << " = " << result.gcd << '\n';
}
