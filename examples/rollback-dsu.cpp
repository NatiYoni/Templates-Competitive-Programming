#include "toolkit/kactl_prelude.hpp"
#include "content/data-structures/UnionFindRollback.h"

// KACTL RollbackUF (Lukas Polacek, Simon Lindholm; CC0; Source: folklore).
// n >= 0, vertices [0,n). Snapshot using time(), rollback only to a snapshot
// still on the current history branch; never fabricate a history offset.
// Union by size without path compression: find/size/join O(log(n+1)),
// rollback O(number of removed history entries), space O(n).
// Input: join 0-1, snapshot, join 1-2, then undo the second join. Output: 3 2 0
int main() {
    RollbackUF uf(4);
    uf.join(0, 1);
    int snapshot = uf.time();
    uf.join(1, 2);
    cout << uf.size(0) << ' ';
    uf.rollback(snapshot);
    cout << uf.size(0) << ' ' << (uf.find(0) == uf.find(2)) << '\n';
}
