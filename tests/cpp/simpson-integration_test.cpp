#include "toolkit/kactl_prelude.hpp"
#include "content/numerical/Integrate.h"
#include "test_util.hpp"

int main() {
    for (int t = 0; t < 300; ++t) {
        ++cases_checked;
        vector<int> coefficients(4);
        for (int& c : coefficients) c = int(rng() % 13) - 6;
        double a = (int(rng() % 21)-10) / 2.0, b = (int(rng() % 21)-10) / 2.0;
        int n = 1 + rng() % 30;
        int evaluations = 0;
        string input = "c=" + show(coefficients) + " a=" + to_string(a) +
                       " b=" + to_string(b) + " n=" + to_string(n);
        auto f = [&](double x) {
            ++evaluations;
            double value = 0;
            for (int i = 3; i >= 0; --i) value = value*x + coefficients[i];
            require(isfinite(value), input + " finite integrand");
            return value;
        };
        long double expected = 0;
        for (int i = 0; i < 4; ++i)
            expected += coefficients[i] * (powl(b, i+1) - powl(a, i+1)) / (i+1);
        double actual = quad(a, b, f, n);
        require(isfinite(actual) && abs(actual - expected) <= 1e-10L * (1 + abs(expected)),
                input + " analytic cubic");
        require(evaluations == 2*n+1, input + " evaluations");
    }
    for (int n : {1, 2, 3, 10, 31, 100}) {
        ++cases_checked;
        auto quartic = [](double x) { return x*x*x*x; };
        double coarse = abs(quad(0, 1, quartic, n) - 0.2);
        double refined = abs(quad(0, 1, quartic, 2*n) - 0.2);
        require(refined < coarse && abs(coarse / refined - 16) < 0.01,
                "f=x^4 interval=[0,1] n=" + to_string(n) + " refinement");
    }
    ++cases_checked;
    require(abs(quad(0, acos(-1.0), [](double x) { return sin(x); }) - 2) < 1e-10,
            "f=sin(x) interval=[0,pi] default n=1000");
    ++cases_checked;
    require(abs(quad(1, 0, [](double x) { return exp(x); }, 200) - (1-exp(1.0))) < 1e-10,
            "f=exp(x) reversed interval=[1,0] n=200");
    ++cases_checked;
    require(quad(3, 3, [](double) { return 7.0; }, 1) == 0,
            "f=7 interval=[3,3] n=1");
    success();
}
