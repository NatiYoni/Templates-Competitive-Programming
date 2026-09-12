#include "toolkit/subtraction_games.hpp"

int main() {
    toolkit::SubtractionGame game(100, {1, 3, 4});
    cout << "g(10)=" << game.grundy(10) << '\n';
    vector<size_t> heaps{10, 7, 4};
    auto choice = game.winning_move(heaps);
    if (choice) cout << "heap " << choice->heap << ": remove " << choice->removed << '\n';
    else cout << "losing position\n";
}
