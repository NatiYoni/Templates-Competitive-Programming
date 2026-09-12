#include "toolkit/monotone_queue.hpp"

// Fixed width-3 windows of [4,1,1,5,2]. Expected min/max pairs: 1:4 1:5 1:5
int main() {
    auto result = toolkit::sliding_window_extrema({4, 1, 1, 5, 2}, 3);
    for (size_t i = 0; i < result.minimum.size(); ++i)
        cout << (i ? " " : "") << result.minimum[i] << ':' << result.maximum[i];
    cout << '\n';
}
