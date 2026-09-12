#include "toolkit/base.hpp"
#include <atcoder/twosat.hpp>

// ACL two_sat (CC0): add_clause(i,f,j,g) means (x[i]==f) OR (x[j]==g).
// Variables [0,n), n >= 0; answer is meaningful only after satisfiable()==true.
// Re-solving after adding clauses is supported. O(n+m) time/space for m clauses;
// implication-graph Tarjan recursion can be O(n) deep.
// Input forces x0=true and x1=false. Output: 1 1 0
int main() {
    atcoder::two_sat formula(2);
    formula.add_clause(0, true, 0, true);
    formula.add_clause(0, false, 1, false);
    bool possible = formula.satisfiable();
    cout << possible;
    if (possible) {
        auto assignment = formula.answer();
        for (bool value : assignment) cout << ' ' << value;
    }
    cout << '\n';
}
