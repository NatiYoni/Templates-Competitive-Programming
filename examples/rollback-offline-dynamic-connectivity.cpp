#include "toolkit/offline_connectivity.hpp"

// Parallel undirected copies survive one removal. Expected: 1 0 1
int main() {
    using A = toolkit::ConnectivityAction;
    vector<toolkit::ConnectivityEvent> events{
        {A::add, 0, 1}, {A::add, 1, 0}, {A::remove, 0, 1},
        {A::query, 0, 1}, {A::remove, 1, 0}, {A::query, 0, 1}, {A::query, 2, 2}};
    auto answer = toolkit::offline_connectivity(3, events);
    for (size_t i = 0; i < answer.size(); ++i) cout << (i ? " " : "") << answer[i];
    cout << '\n';
}
