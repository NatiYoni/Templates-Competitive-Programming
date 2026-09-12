#include "toolkit/kactl_prelude.hpp"
#include "content/strings/AhoCorasick.h"
#include "test_util.hpp"

void check(vector<string> patterns, const string& text) {
    ++cases_checked;
    string input = "patterns=" + show(patterns) + " text=" + text;
    auto before = patterns;
    AhoCorasick ac(patterns);
    require(patterns == before, input + " construction mutation");
    vi longest(text.size(), -1);
    vector<vi> all_matches(text.size());
    for (int i = 0; i < int(text.size()); ++i)
        for (int j = 0; j < int(patterns.size()); ++j)
            if (text.substr(i, patterns[j].size()) == patterns[j]) {
                all_matches[i].push_back(j);
                int end = i + int(patterns[j].size()) - 1;
                int old = longest[end];
                if (old == -1 || patterns[j].size() >= patterns[old].size()) longest[end] = j;
            }
    require(ac.find(text) == longest, input + " longest expected=" + show(longest));
    auto actual = ac.findAll(patterns, text);
    require(actual.size() == text.size(), input + " shape");
    for (int i = 0; i < int(text.size()); ++i) {
        for (int j = 1; j < int(actual[i].size()); ++j)
            require(patterns[actual[i][j-1]].size() <= patterns[actual[i][j]].size(),
                    input + " start=" + to_string(i) + " shortest-first");
        sort(actual[i].begin(), actual[i].end());
        require(actual[i] == all_matches[i], input + " start=" + to_string(i));
    }
    require(ac.find(text) == longest && patterns == before, input + " repeat queries");
}

int main() {
    check({}, "");
    check({}, "AZZA");
    check({"A"}, "");
    check({"A", "AA", "A", "AAA", "AA"}, "AAAAA");
    check({"HE", "SHE", "HERS", "HIS"}, "AHISHERS");
    check({"AZ", "Z", "AZ"}, "AZZAZ");
    for (int t = 0; t < 300; ++t) {
        vector<string> patterns(rng() % 9);
        for (string& p : patterns) {
            p.assign(1 + rng() % 6, 'A');
            for (char& c : p) c += rng() % 3;
        }
        if (patterns.size() > 1 && t % 2 == 0) patterns.back() = patterns.front();
        string text(rng() % 31, 'A');
        for (char& c : text) c += rng() % 3;
        check(patterns, text);
    }
    success();
}
