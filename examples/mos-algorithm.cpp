#include "toolkit/mo_distinct.hpp"

// Distinct values in half-open ranges [0,4),[1,3),[2,2). Expected: 3 2 0
int main() {
    auto answer = toolkit::mo_distinct({7, 2, 7, 9}, {{0, 4}, {1, 3}, {2, 2}});
    for (size_t i = 0; i < answer.size(); ++i) cout << (i ? " " : "") << answer[i];
    cout << '\n';
}
