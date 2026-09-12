#pragma once
#include "base.hpp"

namespace toolkit {
// Independently authored toolkit types/validation; no upstream code copied.
enum class DistanceState { unreachable, finite, negative_infinity };
struct SignedDistance {
    DistanceState state = DistanceState::unreachable;
    optional<long long> value;
};
struct SignedEdge { int from, to; long long cost; };
inline void validate_signed_graph(int n, const vector<SignedEdge>& edges) {
    if (n < 0) throw invalid_argument("negative graph size");
    long long bound = (LLONG_MAX - 1) / max(1, n);
    for (auto e : edges) {
        if (e.from < 0 || e.from >= n || e.to < 0 || e.to >= n)
            throw out_of_range("signed graph endpoint");
        if (e.cost < -bound || e.cost > bound)
            throw invalid_argument("signed graph cost bound");
    }
}
inline void validate_path_source(int n, int source) {
    if (source < 0 || source >= n) throw out_of_range("shortest path source");
}
}
