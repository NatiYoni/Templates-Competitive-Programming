#include "test_util.hpp"
#include "toolkit/offline_connectivity.hpp"

using A = toolkit::ConnectivityAction;
using E = toolkit::ConnectivityEvent;
void run(int n, int steps) {
    ++cases_checked;
    vector<vector<int>> count(n, vector<int>(n));
    vector<E> events;
    vector<bool> expected;
    string input = "n=" + to_string(n);
    for (int t = 0; t < steps && n; ++t) {
        int u = int(rng() % n), v = int(rng() % n), type = int(rng() % 3);
        if (type == 1 && count[u][v] == 0) type = 0;
        events.push_back({type == 0 ? A::add : type == 1 ? A::remove : A::query, u, v});
        input += ";" + to_string(type) + "," + to_string(u) + "," + to_string(v);
        if (type < 2) {
            int delta = type == 0 ? 1 : -1;
            count[u][v] += delta;
            if (u != v) count[v][u] += delta;
        } else {
            vector<int> seen(n);
            queue<int> todo;
            todo.push(u); seen[u] = 1;
            while (!todo.empty()) {
                int a = todo.front(); todo.pop();
                for (int b = 0; b < n; ++b) if (count[a][b] && !seen[b]) {
                    seen[b] = 1; todo.push(b);
                }
            }
            expected.push_back(seen[v] != 0);
        }
    }
    require(toolkit::offline_connectivity(n, events) == expected, input);
    require(toolkit::offline_connectivity(n, events) == expected, input + ";repeat local lifecycle");
}
int main() {
    run(0, 0); run(1, 100);
    for (int t = 0; t < 260; ++t) run(1 + int(rng() % 9), 100);
    ++cases_checked;
    vector<E> events{{A::add,0,1},{A::add,1,0},{A::remove,0,1},{A::query,0,1},
                     {A::remove,1,0},{A::query,0,1},{A::add,0,1},{A::query,0,1}};
    require(toolkit::offline_connectivity(2, events) == vector<bool>({true,false,true}),
            "duplicate reversed edge remove/readd");
    ++cases_checked;
    for (auto bad : vector<vector<E>>{{{A::remove,0,1}},
             {{A::add,0,0},{A::remove,0,0},{A::remove,0,0}}}) {
        bool rejected = false;
        try { toolkit::offline_connectivity(2, bad); }
        catch (const invalid_argument&) { rejected = true; }
        require(rejected, "unmatched removal timeline length=" + to_string(bad.size()));
    }
    ++cases_checked;
    bool rejected = false;
    try { toolkit::offline_connectivity(0, {{A::query,0,0}}); }
    catch (const out_of_range&) { rejected = true; }
    require(rejected, "empty graph invalid vertex");
    success();
}
