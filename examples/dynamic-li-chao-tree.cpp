#include "toolkit/dynamic_li_chao.hpp"

// Min of 2x+3 and -x+4 at x=-2,0,2. Expected: -1 3 2
int main() {
    toolkit::DynamicLiChaoMin lines(-10, 10);
    lines.add(2, 3);
    lines.add(-1, 4);
    for (int x : {-2, 0, 2})
        cout << (x == -2 ? "" : " ") << (long long)*lines.query(x);
    cout << '\n'; // Fixed small values fit; general evaluation is __int128.
}
