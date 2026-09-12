#include "test_util.hpp"
#include "toolkit/persistent_segment_tree.hpp"

void run(int n) {
    ++cases_checked;
    toolkit::PersistentRangeSum tree(n);
    vector<vector<long long>> versions(1, vector<long long>(n));
    string input = "n=" + to_string(n);
    for (int step = 0; step < 60; ++step) {
        size_t version = rng() % versions.size();
        if (n) {
            int pos = int(rng() % n);
            long long x = int(rng() % 101) - 50;
            input += ";set(" + to_string(version) + "," + to_string(pos) + "," + to_string(x) + ")";
            auto copy = versions[version];
            copy[pos] = x;
            versions.push_back(copy);
            require(tree.set(version, pos, x) == versions.size() - 1, input);
        }
        require(tree.versions() == versions.size(), input);
        for (size_t v = 0; v < versions.size(); ++v) {
            int l = int(rng() % (n + 1)), r = int(rng() % (n + 1));
            if (l > r) swap(l, r);
            __int128 sum = 0;
            for (int i = l; i < r; ++i) sum += versions[v][i];
            require(tree.sum(v, l, r) == sum, input + ";query=" + to_string(v) +
                    "," + to_string(l) + "," + to_string(r));
        }
    }
}
int main() {
    run(0); run(1);
    for (int t = 0; t < 250; ++t) run(int(rng() % 20));
    ++cases_checked;
    toolkit::PersistentRangeSum wide(3);
    auto v1 = wide.set(0, 0, LLONG_MAX), v2 = wide.set(v1, 1, LLONG_MAX);
    auto v3 = wide.set(v2, 2, LLONG_MIN);
    require(wide.sum(v2, 0, 3) == (__int128)LLONG_MAX * 2 &&
            wide.sum(v3, 0, 3) == (__int128)LLONG_MAX * 2 + LLONG_MIN, "signed-wide versions");
    ++cases_checked;
    toolkit::PersistentRangeSum bounded(4, 3);
    bool rejected = false;
    try { bounded.set(0, 1, 8); } catch (const length_error&) { rejected = true; }
    require(rejected && bounded.versions() == 1 && bounded.node_count() == 1 &&
            bounded.sum(0, 0, 4) == 0, "node budget atomic failure");
    ++cases_checked;
    for (int action = 0; action < 4; ++action) {
        rejected = false;
        try {
            if (action == 0) wide.set(99, 0, 1);
            if (action == 1) wide.set(0, 3, 1);
            if (action == 2) wide.sum(0, 2, 1);
            if (action == 3) wide.sum(0, 0, 4);
        } catch (const out_of_range&) { rejected = true; }
        require(rejected, "invalid action=" + to_string(action));
    }
    ++cases_checked;
    toolkit::PersistentRangeSum sparse(1000000000);
    auto v = sparse.set(0, 999999999, -7);
    require(sparse.sum(v, 0, 1000000000) == -7 && sparse.node_count() <= 32,
            "sparse maximum dimension");
    success();
}
