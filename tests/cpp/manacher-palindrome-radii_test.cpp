#include "toolkit/palindrome_radii.hpp"
#include "test_util.hpp"

string bytes(const string& s) {
    vector<int> out;
    for (unsigned char c : s) out.push_back(c);
    return show(out);
}
bool palindrome(const string& s, int l, int r) {
    for (--r; l < r; ++l, --r) if (s[l] != s[r]) return false;
    return true;
}
void check(const string& s) {
    ++cases_checked;
    auto actual = toolkit::palindrome_radii(s);
    string input = "bytes=" + bytes(s);
    require(actual.odd.size() == s.size() && actual.even.size() == s.size(), input + " sizes");
    long long total = 0, brute = 0;
    for (int i = 0, n = int(s.size()); i < n; ++i) {
        int odd = 0, even = 0;
        for (int radius = 1; radius <= min(i + 1, n - i); ++radius)
            if (palindrome(s, i - radius + 1, i + radius)) odd = radius;
        for (int radius = 1; radius <= min(i, n - i); ++radius)
            if (palindrome(s, i - radius, i + radius)) even = radius;
        require(actual.odd[i] == odd && actual.even[i] == even,
                input + " center=" + to_string(i) + " odd=" + show(actual.odd) +
                " even=" + show(actual.even));
        total += actual.odd[i] + actual.even[i];
        for (int j = i + 1; j <= n; ++j) brute += palindrome(s, i, j);
    }
    require(total == brute, input + " all nonempty palindrome intervals");
}
int main() {
    for (const string& s : vector<string>{"", "a", "abba", "abacaba", "aaaaaa",
                                         string("\0\xff\0\xff", 4)}) check(s);
    string alphabet;
    for (int c = 0; c < 256; ++c) alphabet.push_back(char(c));
    check(alphabet);
    for (int n = 0; n <= 8; ++n)
        for (int mask = 0; mask < (1 << n); ++mask) {
            string s(n, 'a');
            for (int i = 0; i < n; ++i) s[i] += (mask >> i) & 1;
            check(s);
        }
    for (int t = 0; t < 350; ++t) {
        string s(rng() % 41, '\0');
        for (char& c : s) c = char(rng() % (t % 2 ? 4 : 256));
        check(s);
    }
    success();
}
