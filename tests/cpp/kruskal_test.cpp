#include "toolkit/kruskal.hpp"
#include "test_util.hpp"

bool join(vector<int>& labels, int u, int v) {
    int from = labels[v], to = labels[u];
    if (from == to) return false;
    for (int& label : labels) if (label == from) label = to;
    return true;
}

int main() {
    for (int trial = 0; trial < 350; ++trial) {
        int n = rng() % 6, m = n ? rng() % 9 : 0;
        vector<toolkit::WeightedEdge> edges;
        ostringstream input;
        input << "n=" << n << " edges=";
        for (int i = 0; i < m; ++i) {
            int u = rng() % n, v = rng() % n;
            long long w = int(rng() % 11) - 5;
            edges.push_back({u, v, w});
            input << '(' << u << ',' << v << ',' << w << ')';
        }
        vector<int> original(n);
        iota(original.begin(), original.end(), 0);
        int components = n;
        for (auto e : edges) components -= join(original, e.u, e.v);
        long long best = LLONG_MAX;
        for (int mask = 0; mask < (1 << m); ++mask) {
            vector<int> labels(n);
            iota(labels.begin(), labels.end(), 0);
            int count = n;
            long long weight = 0;
            bool forest = true;
            for (int i = 0; i < m; ++i) if (mask >> i & 1) {
                auto e = edges[i];
                if (!join(labels, e.u, e.v)) { forest = false; break; }
                --count;
                weight += e.weight;
            }
            if (forest && count == components) best = min(best, weight);
        }
        ++cases_checked;
        auto result = toolkit::kruskal(n, edges);
        require(result.weight == best && result.components == components, input.str());
        vector<int> labels(n);
        iota(labels.begin(), labels.end(), 0);
        long long selected_weight = 0;
        pair<long long, int> previous{LLONG_MIN, -1};
        for (int i : result.edges) {
            require(0 <= i && i < m, input.str() + " selected index=" + to_string(i));
            auto e = edges[i];
            require(join(labels, e.u, e.v) && previous < make_pair(e.weight, i), input.str());
            previous = {e.weight, i};
            selected_weight += e.weight;
        }
        require(int(result.edges.size()) == n - components && selected_weight == best, input.str());
    }
    for (long long weight : {LLONG_MIN, LLONG_MAX}) {
        ++cases_checked;
        auto f = toolkit::kruskal(2, {{0, 1, weight}});
        require(f.weight == weight && f.components == 1 && f.edges == vector<int>{0},
                "n=2 single weight=" + to_string(weight));
    }
    ++cases_checked;
    require(toolkit::kruskal(3, {{0, 1, 1}, {1, 2, 1}, {0, 2, 1}}).edges == vector<int>({0, 1}),
            "equal-weight triangle chooses input indices 0,1");
    for (auto weights : {pair<long long, long long>{LLONG_MIN, -1}, {LLONG_MAX, 1}}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::kruskal(3, {{0, 1, weights.first}, {1, 2, weights.second}}); }
        catch (const overflow_error&) { rejected = true; }
        require(rejected, "n=3 chain weights=" + to_string(weights.first)
                          + "," + to_string(weights.second));
    }
    for (int to : {-1, 2}) {
        ++cases_checked;
        bool rejected = false;
        try { toolkit::kruskal(2, {{0, to, 1}}); }
        catch (const out_of_range&) { rejected = true; }
        require(rejected, "n=2 edge 0->" + to_string(to));
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::kruskal(-1, {}); }
    catch (const invalid_argument&) { rejected = true; }
    require(rejected, "n=-1 edges=[]");
    success();
}
