#include "test_util.hpp"
#include <atcoder/dsu.hpp>

// Independent oracle: explicit partition labels, merged by scanning all labels.
void check_partition(atcoder::dsu& uf, const vector<int>& label, const string& input) {
    int n = int(label.size());
    for (int a = 0; a < n; ++a) {
        int expected_size = int(count(label.begin(), label.end(), label[a]));
        require(uf.size(a) == expected_size, input + " size vertex=" + to_string(a));
        int root = uf.leader(a);
        require(0 <= root && root < n && label[root] == label[a],
                input + " leader vertex=" + to_string(a));
        for (int b = 0; b < n; ++b)
            require(uf.same(a, b) == (label[a] == label[b]),
                    input + " same=" + to_string(a) + "," + to_string(b));
    }
    map<int, vector<int>> by_label;
    for (int v = 0; v < n; ++v) by_label[label[v]].push_back(v);
    vector<vector<int>> expected, actual = uf.groups();
    for (auto& item : by_label) expected.push_back(item.second);
    for (auto& group : actual) sort(group.begin(), group.end());
    sort(actual.begin(), actual.end());
    sort(expected.begin(), expected.end());
    require(actual == expected, input + " groups labels=" + show(label));
}

void run_case(int n) {
    ++cases_checked;
    atcoder::dsu uf(n);
    vector<int> label(n);
    iota(label.begin(), label.end(), 0);
    string input = "n=" + to_string(n);
    check_partition(uf, label, input);
    for (int step = 0; n && step < 50; ++step) {
        int a = int(rng() % n), b = step % 7 == 0 ? a : int(rng() % n);
        input += ";merge(" + to_string(a) + "," + to_string(b) + ")";
        int old = label[b], replacement = label[a];
        for (int& x : label) if (x == old) x = replacement;
        int root = uf.merge(a, b);
        require(root == uf.leader(a) && root == uf.leader(b), input + " merge return");
        check_partition(uf, label, input);
    }
}

int main() {
    run_case(0);
    run_case(1);
    for (int i = 0; i < 260; ++i) run_case(int(rng() % 13));
    success();
}
