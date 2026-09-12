#include "toolkit/suffix_automaton.hpp"
#include "test_util.hpp"

string bytes(const string& s) {
    vector<int> out;
    for (unsigned char c : s) out.push_back(c);
    return show(out);
}
long long count_pattern(const string& s, const string& p) {
    long long count = 0;
    for (size_t i = 0; i <= s.size(); ++i)
        if (p.size() <= s.size() - i && s.compare(i, p.size(), p) == 0) ++count;
    return count;
}
void check(const string& s) {
    ++cases_checked;
    map<string, long long> frequency;
    for (size_t i = 0; i < s.size(); ++i)
        for (size_t j = i + 1; j <= s.size(); ++j) ++frequency[s.substr(i, j - i)];
    string mutable_input = s;
    toolkit::SuffixAutomaton original(mutable_input);
    mutable_input.assign("changed after construction");
    auto survivor = [&] {
        toolkit::SuffixAutomaton local = original;
        return local;
    }();
    toolkit::SuffixAutomaton unrelated("zzzyzxzy");
    string input = "text=" + bytes(s);
    require(survivor.distinct_substrings() == (long long)frequency.size(), input + " distinct");
    for (const auto& [pattern, count] : frequency)
        require(survivor.contains(pattern) && survivor.occurrences(pattern) == count,
                input + " pattern=" + bytes(pattern) + " count=" + to_string(count));
    vector<string> probes{"", s, s + char(255), "abcbc", string("\0\xff", 2)};
    for (int c = 0; c < 256; ++c) probes.push_back(string(1, char(c)));
    for (const auto& pattern : probes) {
        long long expected = count_pattern(s, pattern);
        require(survivor.occurrences(pattern) == expected &&
                survivor.contains(pattern) == (expected > 0),
                input + " probe=" + bytes(pattern) + " expected=" + to_string(expected));
    }
    require(unrelated.occurrences("z") == 5, input + " unrelated text=zzzyzxzy pattern=z expected=5");
}
int main() {
    for (const string& s : vector<string>{"", "a", "aaaaaa", "ababa", "abcbc", "banana",
                                         string("\0\xff\0\xff\0", 5)}) check(s);
    for (int n = 0; n <= 7; ++n)
        for (int mask = 0; mask < (1 << n); ++mask) {
            string s(n, 'a');
            for (int i = 0; i < n; ++i) s[i] += (mask >> i) & 1;
            check(s);
        }
    for (int t = 0; t < 320; ++t) {
        string s(rng() % 25, '\0');
        for (char& c : s) c = char(rng() % (t % 3 ? 3 : 256));
        check(s);
    }
    auto owned = [] {
        toolkit::SuffixAutomaton source("ababa");
        return toolkit::SuffixAutomaton(source);
    }();
    vector<toolkit::SuffixAutomaton> copies{owned};
    for (int i = 0; i < 20; ++i) copies.push_back(owned);
    for (const auto& copy : copies)
        require(copy.occurrences("aba") == 2, "copy/move/relocation from destroyed source ababa");
    {
        toolkit::SuffixAutomaton source("banana");
        owned = source;
    }
    require(owned.occurrences("ana") == 2 && owned.distinct_substrings() == 15,
            "copy-assignment from destroyed source banana pattern=ana expected=2 distinct=15");
    success();
}
