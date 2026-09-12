#pragma once
#include "base.hpp"

namespace toolkit {
struct WindowExtrema { vector<long long> minimum, maximum; };

// All full windows of width w, 1<=w<=a.size(); invalid widths throw.
// Front indices expire before insertion; equal values keep the newest index.
// O(n) time, O(w) deque storage plus O(n-w+1) output; input unchanged.
inline WindowExtrema sliding_window_extrema(const vector<long long>& a, size_t w) {
    if (w == 0 || w > a.size()) throw invalid_argument("window width");
    deque<size_t> low, high;
    WindowExtrema answer;
    for (size_t i = 0; i < a.size(); ++i) {
        if (i >= w) {
            while (!low.empty() && low.front() <= i - w) low.pop_front();
            while (!high.empty() && high.front() <= i - w) high.pop_front();
        }
        while (!low.empty() && a[low.back()] >= a[i]) low.pop_back();
        while (!high.empty() && a[high.back()] <= a[i]) high.pop_back();
        low.push_back(i);
        high.push_back(i);
        if (i >= w - 1) {
            answer.minimum.push_back(a[low.front()]);
            answer.maximum.push_back(a[high.front()]);
        }
    }
    return answer;
}
}
