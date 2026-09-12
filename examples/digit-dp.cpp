#include "toolkit/digit_state_count.hpp"

// Canonical digits: no equal neighbors, digit sum divisible by 3, [0,20].
// Qualifying numbers are 0,3,6,9,12,15,18. Expected: 7
int main() {
    auto count = toolkit::count_no_adjacent_digit_sum(0, 20, 3, 0);
    cout << (unsigned long long)count << '\n'; // This small count fits.
}
