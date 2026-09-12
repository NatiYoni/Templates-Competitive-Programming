#include "test_util.hpp"
#include "toolkit/kactl_prelude.hpp"
#include "content/graph/HopcroftKarp.h"

// Independent oracle: enumerate unmatched/matched choices for each left vertex.
int enumerate_matchings(const vector<vi>& graph, int right_count) {
    vector<bool> used(right_count);
    auto visit = [&](auto&& self, int u) -> int {
        if (u == int(graph.size())) return 0;
        int best = self(self, u + 1);
        for (int v : graph[u]) if (!used[v]) {
            used[v] = true;
            best = max(best, 1 + self(self, u + 1));
            used[v] = false;
        }
        return best;
    };
    return visit(visit, 0);
}
void run_case(vector<vi> graph, int right_count) {
    ++cases_checked;
    string input = "L=" + to_string(graph.size()) + " R=" + to_string(right_count) + " adjacency=";
    for (const auto& row : graph) input += show(row);
    int expected = enumerate_matchings(graph, right_count);
    auto original = graph;
    for (int repeat = 0; repeat < 2; ++repeat) {
        vi match(right_count, -1), occurrences(graph.size());
        int result = hopcroftKarp(graph, match);
        require(result == expected, input + " cardinality expected=" + to_string(expected));
        require(graph == original, input + " adjacency mutation");
        int matched = 0;
        for (int v = 0; v < right_count; ++v) if (match[v] != -1) {
            int u = match[v];
            require(0 <= u && u < int(graph.size()), input + " invalid match=" + show(match));
            require(++occurrences[u] == 1, input + " repeated left match=" + show(match));
            require(find(graph[u].begin(), graph[u].end(), v) != graph[u].end(),
                    input + " nonexistent matched edge right=" + to_string(v) + " match=" + show(match));
            ++matched;
        }
        require(matched == result, input + " right match count=" + show(match));
    }
}
int main() {
    run_case({}, 0);
    run_case({}, 4);
    run_case(vector<vi>(4), 0);
    run_case({{0, 0}, {0, 1}, {1}}, 2);
    for (int mask = 0; mask < (1 << 9); ++mask) {
        vector<vi> graph(3);
        for (int u = 0; u < 3; ++u)
            for (int v = 0; v < 3; ++v)
                if (mask & (1 << (3 * u + v))) graph[u].push_back(v);
        run_case(graph, 3);
    }
    for (int i = 0; i < 260; ++i) {
        int left = int(rng() % 7), right = int(rng() % 7);
        vector<vi> graph(left);
        for (auto& row : graph)
            for (int v = 0; v < right; ++v) if (rng() % 3 == 0) row.push_back(v);
        run_case(graph, right);
    }
    success();
}
