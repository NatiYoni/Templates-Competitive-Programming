#include "toolkit/topological_sort.hpp"

// Input (fixed): 0->2, 1->2, 2->3; then a self-loop. Expected:
// 0 1 2 3
// cycle
int main() {
    auto order = toolkit::topological_sort({{2}, {2}, {3}, {}});
    if (!order) return 1;
    for (size_t i = 0; i < order->size(); ++i) cout << (i ? " " : "") << (*order)[i];
    cout << '\n';
    if (!toolkit::topological_sort({{0}})) cout << "cycle\n";
}
