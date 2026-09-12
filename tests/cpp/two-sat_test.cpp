#include "test_util.hpp"
#include <atcoder/twosat.hpp>

struct Clause { int i; bool f; int j; bool g; };
// Independent oracle: enumerate every assignment, evaluate the clauses literally.
bool accepts(int assignment, const vector<Clause>& clauses) {
    for (auto c : clauses)
        if (bool((assignment >> c.i) & 1) != c.f &&
            bool((assignment >> c.j) & 1) != c.g) return false;
    return true;
}
void check(atcoder::two_sat& formula, int n, const vector<Clause>& clauses, const string& input) {
    bool possible = false;
    for (int assignment = 0; assignment < (1 << n); ++assignment)
        possible = possible || accepts(assignment, clauses);
    require(formula.satisfiable() == possible, input + " satisfiable");
    if (possible) {
        auto answer = formula.answer();
        require(int(answer.size()) == n, input + " answer length");
        int assignment = 0;
        for (int v = 0; v < n; ++v) if (answer[v]) assignment |= 1 << v;
        require(accepts(assignment, clauses), input + " answer=" + to_string(assignment));
    }
}
void run_case(int n, const vector<Clause>& clauses) {
    ++cases_checked;
    atcoder::two_sat formula(n);
    vector<Clause> prefix;
    string input = "n=" + to_string(n);
    check(formula, n, prefix, input);
    for (auto c : clauses) {
        formula.add_clause(c.i, c.f, c.j, c.g);
        prefix.push_back(c);
        input += ";(" + to_string(c.i) + "==" + to_string(c.f) + " OR " +
                 to_string(c.j) + "==" + to_string(c.g) + ")";
        check(formula, n, prefix, input);
    }
    check(formula, n, clauses, input + ";repeat_solve");
}
int main() {
    run_case(0, {});
    run_case(1, {{0, true, 0, true}, {0, false, 0, false}});
    run_case(1, {{0, true, 0, false}});
    run_case(2, {{0, true, 0, true}, {0, false, 1, false}});
    for (int i = 0; i < 260; ++i) {
        int n = int(rng() % 8), m = n ? int(rng() % 23) : 0;
        vector<Clause> clauses;
        while (m--) clauses.push_back({int(rng() % n), bool(rng() & 1),
                                       int(rng() % n), bool(rng() & 1)});
        run_case(n, clauses);
    }
    success();
}
