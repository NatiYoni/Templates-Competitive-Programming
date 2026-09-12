#include "toolkit/longest_common_subsequence.hpp"
#include "test_util.hpp"

string bytes(const string& s) {
    vector<int> out;
    for (unsigned char c : s) out.push_back(c);
    return show(out);
}
bool subsequence(const string& candidate, const string& text) {
    size_t i = 0;
    for (char c : text) if (i < candidate.size() && c == candidate[i]) ++i;
    return i == candidate.size();
}
size_t exhaustive_length(string a, string b) {
    if (a.size() > b.size()) swap(a, b);
    size_t best = 0;
    for (unsigned mask = 0; mask < (1u << a.size()); ++mask) {
        string chosen;
        for (size_t i = 0; i < a.size(); ++i) if ((mask >> i) & 1) chosen.push_back(a[i]);
        if (subsequence(chosen, b)) best = max(best, chosen.size());
    }
    return best;
}
void check(const string& a, const string& b) {
    ++cases_checked;
    string result = toolkit::longest_common_subsequence(a, b);
    string input = "a=" + bytes(a) + " b=" + bytes(b) + " result=" + bytes(result);
    require(subsequence(result, a) && subsequence(result, b), input + " reconstruction");
    require(result.size() == exhaustive_length(a, b), input + " exhaustive optimum");
    require(toolkit::longest_common_subsequence(b, a).size() == result.size(), input + " symmetric length");
}
int main() {
    check("", "");
    check("", "a");
    check("abc", "");
    check("abcde", "ace");
    check("AGGTAB", "GXTXAYB");
    check("abc", "xyz");
    check("abababab", "babababa");
    check(string("\0\xff\x01\0", 4), string("\xff\0", 2));
    vector<string> binary;
    for (int n = 0; n <= 4; ++n)
        for (int mask = 0; mask < (1 << n); ++mask) {
            string s(n, 'a');
            for (int i = 0; i < n; ++i) s[i] += (mask >> i) & 1;
            binary.push_back(s);
        }
    for (const string& a : binary) for (const string& b : binary) check(a, b);
    for (int t = 0; t < 320; ++t) {
        string a(rng() % 12, '\0'), b(rng() % 12, '\0');
        for (char& c : a) c = char(rng() % (t % 4 ? 4 : 256));
        for (char& c : b) c = char(rng() % (t % 4 ? 4 : 256));
        check(a, b);
    }
    success();
}
