#include "toolkit/centroid_nearest.hpp"
#include "test_util.hpp"

string describe(int n, const vector<pair<int, int>>& edges) {
    string s = "n=" + to_string(n) + " edges=";
    for (auto [u, v] : edges) s += "(" + to_string(u) + "," + to_string(v) + ")";
    return s;
}
void check(int n, const vector<pair<int, int>>& edges) {
    ++cases_checked;
    string input = describe(n, edges);
    toolkit::CentroidNearest cd(n, edges);
    vector<vector<int>> g(n), distance(n, vector<int>(n, -1));
    for (auto [u, v] : edges) { g[u].push_back(v); g[v].push_back(u); }
    for (int s = 0; s < n; ++s) {
        vector<int> q{s}; distance[s][s] = 0;
        for (size_t i = 0; i < q.size(); ++i) for (int u : g[q[i]]) if (distance[s][u] == -1) {
            distance[s][u] = distance[s][q[i]] + 1; q.push_back(u);
        }
    }
    int roots = 0;
    for (int c = 0; c < n; ++c) {
        roots += cd.parent()[c] == -1;
        vector<bool> member(n);
        for (int v = 0; v < n; ++v) {
            int x = v, steps = 0;
            while (x != -1 && x != c && steps++ <= n) {
                require(x >= 0 && x < n, input + " centroid parent range");
                x = cd.parent()[x];
            }
            require(steps <= n, input + " centroid parent cycle");
            member[v] = x == c;
        }
        int total = count(member.begin(), member.end(), true);
        // Each child-side component has at most half of the current component.
        vector<bool> seen(n);
        seen[c] = true;
        for (int start : g[c]) if (member[start] && !seen[start]) {
            vector<int> q{start}; seen[start] = true;
            for (size_t i = 0; i < q.size(); ++i)
                for (int u : g[q[i]]) if (member[u] && !seen[u]) { seen[u] = true; q.push_back(u); }
            require(int(q.size()) * 2 <= total, input + " centroid balance c=" + to_string(c));
        }
        require(!cd.nearest(c), input + " initially no marked vertex");
        vector<int> chain;
        for (auto [ancestor, d] : cd.ancestors()[c]) {
            require(ancestor >= 0 && ancestor < n && d == distance[c][ancestor], input + " ancestor distance");
            chain.push_back(ancestor);
        }
        vector<int> expected;
        for (int x = c; x != -1; x = cd.parent()[x]) expected.push_back(x);
        reverse(expected.begin(), expected.end());
        require(chain == expected, input + " ancestor chain");
    }
    require(roots == 1, input + " unique centroid root");
    vector<bool> marked(n);
    vector<int> activations;
    for (int step = 0; step < 35; ++step) {
        if (step == 17) {
            cd.reset(); fill(marked.begin(), marked.end(), false); activations.clear();
            for (int v = 0; v < n; ++v) require(!cd.nearest(v), input + " after reset");
        }
        int x = rng() % n; cd.activate(x); cd.activate(x); marked[x] = true;
        activations.push_back(x);
        for (int v = 0; v < n; ++v) {
            int best = n;
            for (int u = 0; u < n; ++u) if (marked[u]) best = min(best, distance[v][u]);
            require(cd.nearest(v) == optional<int>(best),
                    input + " activations=" + show(activations) + " query=" + to_string(v));
        }
    }
}
int main() {
    for (int t = 0; t < 450; ++t) {
        int n = 1 + rng() % 65;
        vector<int> label(n); iota(label.begin(), label.end(), 0);
        shuffle(label.begin(), label.end(), rng);
        vector<pair<int, int>> edges;
        for (int v = 1; v < n; ++v) {
            int p = t % 5 == 0 ? v - 1 : t % 5 == 1 ? 0 : rng() % v;
            edges.push_back({label[p], label[v]});
        }
        shuffle(edges.begin(), edges.end(), rng);
        check(n, edges);
    }
    for (auto edges : vector<vector<pair<int, int>>>{
            {{0, 0}, {1, 2}}, {{0, 1}, {0, 1}}, {{0, 1}}, {{0, 1}, {1, 2}, {2, 0}}}) {
        int n = edges.size() == 3 ? 4 : 3;
        ++cases_checked; bool rejected = false;
        try { toolkit::CentroidNearest cd(n, edges); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, describe(n, edges));
    }
    ++cases_checked; bool rejected = false;
    try { toolkit::CentroidNearest cd(0, {}); } catch (const invalid_argument&) { rejected = true; }
    require(rejected, "empty tree rejected");
    ++cases_checked; rejected = false;
    try { toolkit::CentroidNearest cd(2, {{0, 2}}); } catch (const out_of_range&) { rejected = true; }
    require(rejected, "tree endpoint outside n=2");
    toolkit::CentroidNearest singleton(1, {});
    for (int v : {-1, 1}) {
        ++cases_checked; rejected = false;
        try { singleton.activate(v); } catch (const out_of_range&) { rejected = true; }
        require(rejected, "singleton invalid activation=" + to_string(v));
        ++cases_checked; rejected = false;
        try { singleton.nearest(v); } catch (const out_of_range&) { rejected = true; }
        require(rejected, "singleton invalid query=" + to_string(v));
    }
    success();
}
