#pragma once
#include "signed_paths.hpp"

namespace toolkit {
// Independent original. Parallel arcs use minimum cost; negative loops retained.
// O(n^3+m) time, O(n^2) space. Empty graph gives an empty matrix.
inline vector<vector<SignedDistance>> floyd_warshall(int n,
                                                    const vector<SignedEdge>& edges) {
    validate_signed_graph(n, edges);
    const __int128 inf = __int128(1) << 100;
    vector<vector<__int128>> d(n, vector<__int128>(n, inf));
    for (int v = 0; v < n; ++v) d[v][v] = 0;
    for (auto e : edges) d[e.from][e.to] = min(d[e.from][e.to], __int128(e.cost));
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i) if (d[i][k] != inf)
            for (int j = 0; j < n; ++j) if (d[k][j] != inf)
                // Negative-cycle walks can grow exponentially. This floor is
                // below every simple path; affected outputs never expose it.
                d[i][j] = min(d[i][j], max(-inf, d[i][k] + d[k][j]));
    vector<vector<SignedDistance>> result(n, vector<SignedDistance>(n));
    for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) {
        bool bad = false;
        for (int k = 0; k < n; ++k)
            bad |= d[i][k] != inf && d[k][k] < 0 && d[k][j] != inf;
        if (bad) result[i][j].state = DistanceState::negative_infinity;
        else if (d[i][j] != inf) result[i][j] = {DistanceState::finite, (long long)d[i][j]};
    }
    return result;
}
}
