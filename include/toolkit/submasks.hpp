#pragma once
#include "base.hpp"

namespace toolkit {
// Visit every uint64_t submask in descending order, including zero exactly once.
// O(2^popcount(mask)) calls, O(1) extra space; full-width masks are valid but
// usually infeasible. No shifts, mutation/global state/recursion; callback
// exceptions propagate and stop enumeration. Mask=0 calls visit(0) once.
template<class Visitor> void for_each_submask(uint64_t mask, Visitor visit) {
    uint64_t submask = mask;
    while (true) {
        visit(submask);
        if (submask == 0) break;
        submask = (submask - 1) & mask;
    }
}
}
