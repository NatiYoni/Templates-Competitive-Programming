#include "test_util.hpp"
#include "toolkit/kactl_prelude.hpp"
#include "content/data-structures/UnionFindRollback.h"

// Independent oracle: full copied label arrays per live snapshot, no DSU logic.
struct Snapshot { int time; vector<int> labels, structure; };
void check(RollbackUF& uf, const vector<int>& labels, const string& input) {
    for (int a = 0; a < int(labels.size()); ++a) {
        require(uf.size(a) == int(count(labels.begin(), labels.end(), labels[a])),
                input + " size=" + to_string(a));
        for (int b = 0; b < int(labels.size()); ++b)
            require((uf.find(a) == uf.find(b)) == (labels[a] == labels[b]),
                    input + " same=" + to_string(a) + "," + to_string(b));
    }
}
void run_case(int n) {
    ++cases_checked;
    RollbackUF uf(n);
    vector<int> labels(n);
    iota(labels.begin(), labels.end(), 0);
    vector<Snapshot> saved{{uf.time(), labels, uf.e}};
    string input = "n=" + to_string(n) + ";snapshot(0)";
    check(uf, labels, input);
    for (int step = 0; step < 80; ++step) {
        int action = int(rng() % 4);
        if (action <= 1 && n) {
            int a = int(rng() % n), b = step % 9 ? int(rng() % n) : a;
            bool expected = labels[a] != labels[b];
            int old_time = uf.time();
            input += ";join(" + to_string(a) + "," + to_string(b) + ")";
            require(uf.join(a, b) == expected, input + " join return");
            require(uf.time() == old_time + (expected ? 2 : 0), input + " history length");
            int previous = labels[b], replacement = labels[a];
            for (int& x : labels) if (x == previous) x = replacement;
        } else if (action == 2) {
            saved.push_back({uf.time(), labels, uf.e});
            input += ";snapshot(" + to_string(uf.time()) + ")";
        } else {
            int index = int(rng() % saved.size());
            Snapshot snapshot = saved[index];
            input += ";rollback(" + to_string(snapshot.time) + ")";
            uf.rollback(snapshot.time);
            labels = snapshot.labels;
            saved.resize(index + 1);  // Future snapshots belong to a discarded branch.
            require(uf.time() == snapshot.time && uf.e == snapshot.structure,
                    input + " exact snapshot restoration labels=" + show(labels));
        }
        check(uf, labels, input);
    }
    uf.rollback(0);
    for (int v = 0; v < n; ++v)
        require(uf.size(v) == 1, input + ";rollback(0) vertex=" + to_string(v));
}
int main() {
    run_case(0);
    run_case(1);
    for (int i = 0; i < 260; ++i) run_case(int(rng() % 15));
    success();
}
