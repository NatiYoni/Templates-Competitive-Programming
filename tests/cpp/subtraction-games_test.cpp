#include "toolkit/subtraction_games.hpp"
#include "test_util.hpp"

size_t grundy_oracle(size_t heap, const vector<size_t>& moves, map<size_t, size_t>& memo) {
    auto found = memo.find(heap);
    if (found != memo.end()) return found->second;
    set<size_t> reachable;
    for (auto move : moves)
        if (move <= heap) reachable.insert(grundy_oracle(heap - move, moves, memo));
    size_t mex = 0;
    while (reachable.count(mex)) ++mex;
    return memo[heap] = mex;
}
bool winning_oracle(const vector<size_t>& heaps, const vector<size_t>& moves,
                    map<vector<size_t>, bool>& memo) {
    auto found = memo.find(heaps);
    if (found != memo.end()) return found->second;
    for (size_t i = 0; i < heaps.size(); ++i)
        for (auto amount : moves) if (amount <= heaps[i]) {
            auto next = heaps;
            next[i] -= amount;
            if (!winning_oracle(next, moves, memo)) return memo[heaps] = true;
        }
    return memo[heaps] = false;
}

int main() {
    for (unsigned mask = 0; mask < 64; ++mask) {
        vector<size_t> moves;
        for (unsigned bit = 0; bit < 6; ++bit) if (mask >> bit & 1) moves.push_back(bit + 1);
        toolkit::SubtractionGame game(30, moves);
        map<size_t, size_t> memo;
        for (size_t heap = 0; heap <= 30; ++heap) {
            ++cases_checked;
            string input = "moves=" + show(moves) + " heap=" + to_string(heap);
            size_t value = grundy_oracle(heap, moves, memo);
            require(game.grundy(heap) == value, input);
            optional<size_t> expected;
            for (auto amount : moves)
                if (amount <= heap && !grundy_oracle(heap - amount, moves, memo)) {
                    expected = amount;
                    break;
                }
            require(game.winning_move(heap) == expected && bool(expected) == (value != 0), input);
        }
    }
    for (int iteration = 0; iteration < 300; ++iteration) {
        vector<size_t> moves, heaps(rng() % 5);
        for (auto& heap : heaps) heap = rng() % 9;
        for (size_t amount = 1; amount <= 7; ++amount) if (rng() & 1) moves.push_back(amount);
        auto noisy = moves;
        noisy.insert(noisy.end(), moves.begin(), moves.end());
        noisy.push_back(100);
        shuffle(noisy.begin(), noisy.end(), rng);
        toolkit::SubtractionGame game(8, noisy);
        map<vector<size_t>, bool> memo;
        bool winning = winning_oracle(heaps, moves, memo);
        auto actual = game.winning_move(heaps);
        ++cases_checked;
        string input = "moves=" + show(noisy) + " heaps=" + show(heaps);
        require((game.nim_sum(heaps) != 0) == winning && bool(actual) == winning, input);
        optional<toolkit::SubtractionMove> expected;
        for (size_t i = 0; i < heaps.size() && !expected; ++i)
            for (auto amount : moves) if (amount <= heaps[i]) {
                auto next = heaps;
                next[i] -= amount;
                if (!winning_oracle(next, moves, memo)) {
                    expected = toolkit::SubtractionMove{i, amount};
                    break;
                }
            }
        require(bool(expected) == bool(actual), input);
        if (actual) require(actual->heap == expected->heap && actual->removed == expected->removed, input);
    }
    ++cases_checked;
    toolkit::SubtractionGame empty(0, {SIZE_MAX, SIZE_MAX});
    require(empty.grundy(0) == 0 && !empty.winning_move(size_t(0)) &&
            !empty.winning_move(vector<size_t>{}), "zero maximum, huge moves, empty sum");
    for (int operation = 0; operation < 5; ++operation) {
        ++cases_checked;
        bool caught = false;
        try {
            if (operation == 0) toolkit::SubtractionGame invalid(10, {1, 0});
            if (operation == 1) toolkit::SubtractionGame invalid(SIZE_MAX, {});
            if (operation == 2) empty.grundy(1);
            if (operation == 3) empty.winning_move(size_t(1));
            if (operation == 4) empty.winning_move(vector<size_t>{0, 1});
        } catch (const logic_error&) { caught = true; }
        require(caught, "invalid subtraction operation=" + to_string(operation));
    }
    success();
}
