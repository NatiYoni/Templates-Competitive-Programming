#include "toolkit/nim.hpp"
#include "test_util.hpp"

map<vector<uint64_t>, bool> memo;
bool oracle(const vector<uint64_t>& heaps) {
    auto it = memo.find(heaps);
    if (it != memo.end()) return it->second;
    for (size_t i = 0; i < heaps.size(); ++i)
        for (uint64_t remaining = 0; remaining < heaps[i]; ++remaining) {
            auto next = heaps;
            next[i] = remaining;
            if (!oracle(next)) return memo[heaps] = true;
        }
    return memo[heaps] = false;
}

int main() {
    int states = 1;
    for (int n = 0; n <= 4; ++n, states *= 4)
        for (int mask = 0; mask < states; ++mask) {
            vector<uint64_t> heaps(n);
            int x = mask;
            for (auto& h : heaps) { h = x % 4; x /= 4; }
            ++cases_checked;
            auto move = toolkit::nim_move(heaps);
            bool winning = oracle(heaps);
            require(toolkit::nim_winning(heaps) == winning && bool(move) == winning, show(heaps));
            if (move) {
                require(move->heap < heaps.size() && move->remaining < heaps[move->heap], show(heaps));
                auto next = heaps;
                next[move->heap] = move->remaining;
                require(!oracle(next), show(heaps) + " move->" + show(next));
                for (size_t i = 0; i < move->heap; ++i)
                    for (uint64_t v = 0; v < heaps[i]; ++v) {
                        next = heaps; next[i] = v;
                        require(oracle(next), show(heaps) + " earlier winning heap=" + to_string(i));
                    }
            }
        }
    for (auto heaps : {vector<uint64_t>{uint64_t(1) << 63, 0},
                       vector<uint64_t>{UINT64_MAX, UINT64_MAX},
                       vector<uint64_t>{UINT64_MAX, uint64_t(1) << 63}}) {
        ++cases_checked;
        uint64_t sum = 0;
        for (auto h : heaps) sum ^= h;
        auto move = toolkit::nim_move(heaps);
        require(bool(move) == (sum != 0) && toolkit::nim_winning(heaps) == (sum != 0), show(heaps));
        if (move) {
            require(move->heap < heaps.size() && move->remaining < heaps[move->heap], show(heaps));
            require((sum ^ heaps[move->heap] ^ move->remaining) == 0, show(heaps));
        }
    }
    success();
}
