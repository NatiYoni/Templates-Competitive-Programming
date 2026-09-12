#pragma once
#include "base.hpp"

namespace toolkit {
// First true in [first,last), or last if none. Predicate must be false then true
// and is never called at last. Inverted domains throw; all long long endpoints fit.
// O(log(domain length + 1)) predicate calls, O(1) space; no mutation/recursion.
template<class Predicate>
long long boundary_search(long long first, long long last, Predicate predicate) {
    if (first > last) throw invalid_argument("inverted search domain");
    while (first < last) {
        auto width = static_cast<unsigned long long>(last)
                   - static_cast<unsigned long long>(first);
        long long mid = first + static_cast<long long>(width / 2);
        if (predicate(mid)) last = mid;
        else first = mid + 1;
    }
    return first;
}
}
