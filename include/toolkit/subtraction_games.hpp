#pragma once
#include "base.hpp"

// Independently authored finite normal-play subtraction games, not misere play.
namespace toolkit {
struct SubtractionMove { size_t heap, removed; };

class SubtractionGame {
    vector<size_t> moves, values;
public:
    // Precompute heaps 0..maximum. Remove one allowed positive amount from
    // exactly one heap; no legal move loses. Duplicates are ignored.
    SubtractionGame(size_t maximum, vector<size_t> allowed) : moves(move(allowed)) {
        if (maximum == SIZE_MAX) throw length_error("maximum+1 overflows");
        if (find(moves.begin(), moves.end(), 0) != moves.end())
            throw invalid_argument("zero is not a subtraction move");
        sort(moves.begin(), moves.end());
        moves.erase(unique(moves.begin(), moves.end()), moves.end());
        values.resize(maximum + 1);
        vector<size_t> seen(moves.size() + 1, SIZE_MAX);
        for (size_t heap = 1; heap <= maximum; ++heap) {
            for (size_t amount : moves) {
                if (amount > heap) break;
                seen[values[heap - amount]] = heap;
            }
            size_t mex = 0;
            while (seen[mex] == heap) ++mex;
            values[heap] = mex;
        }
    }
    size_t grundy(size_t heap) const { return values.at(heap); }
    optional<size_t> winning_move(size_t heap) const {
        if (!grundy(heap)) return nullopt;
        for (size_t amount : moves) {
            if (amount > heap) break;
            if (!values[heap - amount]) return amount;
        }
        throw logic_error("winning state without a winning subtraction");
    }
    size_t nim_sum(const vector<size_t>& heaps) const {
        size_t result = 0;
        for (auto heap : heaps) result ^= grundy(heap);
        return result;
    }
    // First heap, then smallest removal reaching total Grundy xor zero.
    optional<SubtractionMove> winning_move(const vector<size_t>& heaps) const {
        size_t total = nim_sum(heaps);
        if (!total) return nullopt;
        for (size_t i = 0; i < heaps.size(); ++i)
            for (size_t amount : moves) {
                if (amount > heaps[i]) break;
                if ((total ^ values[heaps[i]] ^ values[heaps[i] - amount]) == 0)
                    return SubtractionMove{i, amount};
            }
        throw logic_error("nonzero Grundy xor without a winning move");
    }
};
}
