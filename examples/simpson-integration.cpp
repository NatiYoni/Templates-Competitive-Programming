#include "toolkit/kactl_prelude.hpp"
#include "content/numerical/Integrate.h"

// KACTL quad(a,b,f,n=1000): composite Simpson on 2*n equal subintervals.
// Require 1<=n<=INT_MAX/2; finite endpoints, step, function values and weighted
// sums. f should be continuous, sufficiently smooth for fourth-order error.
// O(n) evaluations/O(1) space; reversed bounds negate the integral. Equal bounds
// still evaluate f. An approximation, not a certified error bound: compare
// refinements and watch singularities, oscillation, cancellation, and rounding.
// Input f(x)=x^3 on [0,2], n=20; output: 4.0000000000
int main() {
    auto cubic = [](double x) { return x*x*x; };
    cout << fixed << setprecision(10) << quad(0, 2, cubic, 20) << '\n';
}
