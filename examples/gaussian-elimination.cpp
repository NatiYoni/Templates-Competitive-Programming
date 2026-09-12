#include "toolkit/kactl_prelude.hpp"
#include "content/numerical/SolveLinear.h"

// KACTL solveLinear(A,b,x): n equations, m variables determined by x.size().
// Rectangular A with m entries/row, b.size()==n; copies needed to preserve A,b.
// Returns numerical rank, or -1 for inconsistency; if rank<m, one arbitrary
// solution with free variables zero. On -1, x is not a solution.
// Full pivoting uses absolute eps=1e-12: finite, well-scaled input is required;
// conditioning/scaling can make rank or residual unreliable. Verify residuals.
// O(n^2*m) time, O(m) auxiliary space, destructive A,b, no recursion.
// Input 2x+y=5, x-y=1; output: 2 2.000000 1.000000
int main() {
    vector<vd> a{{2, 1}, {1, -1}};
    vd b{5, 1}, solution(2);
    int rank = solveLinear(a, b, solution);
    if (rank == -1) {
        cout << "inconsistent\n";
        return 0;
    }
    cout << rank << fixed << setprecision(6);
    for (double value : solution) cout << ' ' << value;
    cout << '\n';
}
