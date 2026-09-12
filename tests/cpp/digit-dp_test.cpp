#include "test_util.hpp"
#include "toolkit/digit_state_count.hpp"

bool accepts(unsigned long long x, int modulus, int residue, bool zero) {
    if (x == 0) return zero && residue == 0;
    string digits = to_string(x);
    int sum = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        if (i && digits[i] == digits[i-1]) return false;
        sum += digits[i]-'0';
    }
    return sum % modulus == residue;
}
void run(unsigned long long lo, unsigned long long hi, int m, int r, bool zero) {
    ++cases_checked;
    unsigned __int128 expected = 0;
    for (unsigned long long x = lo;; ++x) {
        expected += accepts(x,m,r,zero);
        if (x == hi) break; // Do not overflow when hi==ULLONG_MAX.
    }
    string input = "[" + to_string(lo) + "," + to_string(hi) + "];m=" +
                   to_string(m) + ";r=" + to_string(r) + ";zero=" + to_string(zero);
    require(toolkit::count_no_adjacent_digit_sum(lo,hi,m,r,zero) == expected, input);
}
int main() {
    for (int t = 0; t < 300; ++t) {
        unsigned long long lo = rng()%5000, hi = lo+rng()%500;
        int m = 1+int(rng()%15), r = int(rng()%m);
        run(lo,hi,m,r,(rng()&1)!=0);
    }
    for (int m = 1; m <= 10; ++m) {
        run(0,0,m,0,true); run(0,0,m,0,false);
        run(0,120,m,0,true);
    }
    run(98,102,1,0,true); run(998,1002,1,0,true);
    run(ULLONG_MAX-100,ULLONG_MAX,7,3,true);
    run(0,100,200,199,true);
    ++cases_checked;
    // For all lengths 1..19, 9 first choices and 9 choices per later digit.
    unsigned __int128 expected = 1, power = 1;
    for (int length = 1; length <= 19; ++length) { power *= 9; expected += power; }
    require(toolkit::count_no_adjacent_digit_sum(0,9999999999999999999ULL,1,0) == expected,
            "all <=19 digit canonical numbers including zero");
    ++cases_checked;
    for (int bad = 0; bad < 5; ++bad) {
        bool rejected = false;
        try {
            toolkit::count_no_adjacent_digit_sum(bad==0 ? 1 : 0,0,
                bad==1 ? 0 : bad==2 ? 201 : 3,bad==3 ? -1 : bad==4 ? 3 : 0);
        } catch (const invalid_argument&) { rejected = true; }
        require(rejected, "invalid digit parameters case=" + to_string(bad));
    }
    success();
}
