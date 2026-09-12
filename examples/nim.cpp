#include "toolkit/nim.hpp"

// Input (fixed): normal-play heaps [3,4,5]. Expected: winning heap=0 remaining=1
int main() {
    vector<uint64_t> heaps{3, 4, 5};
    auto move = toolkit::nim_move(heaps);
    if (!move) return 1;
    cout << "winning heap=" << move->heap << " remaining=" << move->remaining << '\n';
}
