#include "toolkit/knapsack.hpp"

// Input (fixed): capacity=4, (weight,value)=(0,2),(2,3),(3,4). Expected: 6
int main() {
    cout << toolkit::zero_one_knapsack(4, {{0, 2}, {2, 3}, {3, 4}}) << '\n';
}
