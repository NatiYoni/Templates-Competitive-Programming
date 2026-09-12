#include "toolkit/xor_linear_basis.hpp"
#include "test_util.hpp"

uint64_t random64() { return (uint64_t(rng()) << 32) | rng(); }
void check(const vector<uint64_t>& input, uint64_t offset) {
    ++cases_checked;
    string context = show(input) + " offset=" + to_string(offset);
    toolkit::XorBasis basis;
    set<uint64_t> spans{0};
    for (auto value : input) {
        bool independent = !spans.count(value);
        require(basis.insert(value) == independent, context + " insert=" + to_string(value));
        auto next = spans;
        for (auto v : spans) next.insert(v ^ value);
        spans.swap(next);
        require((size_t(1) << basis.rank()) == spans.size(), context + " rank");
    }
    uint64_t maximum = 0;
    for (auto value : spans) {
        require(basis.contains(value), context + " missing=" + to_string(value));
        maximum = max(maximum, offset ^ value);
    }
    for (int query = 0; query < 15; ++query) {
        uint64_t value = random64();
        if (query < 8) value &= 255;
        require(basis.contains(value) == bool(spans.count(value)), context + " query=" + to_string(value));
    }
    require(basis.maximum_xor() == *spans.rbegin(), context + " maximum");
    require(basis.maximum_xor(offset) == maximum, context + " affine maximum");
    require(basis.contains(0), context + " empty subset");
}

int main() {
    check({}, UINT64_MAX);
    check({0, 0, 0}, 0);
    check({3, 5, 6, 0, 3}, 1);
    for (int iteration = 0; iteration < 450; ++iteration) {
        vector<uint64_t> input(rng() % 12);
        for (auto& value : input) {
            value = random64();
            if (iteration % 3 == 0) value &= 255;
        }
        check(input, random64());
    }
    for (int iteration = 0; iteration < 30; ++iteration) {
        ++cases_checked;
        vector<uint64_t> powers;
        for (int bit = 0; bit < 64; ++bit) powers.push_back(uint64_t(1) << bit);
        shuffle(powers.begin(), powers.end(), rng);
        string context = "full rank order=" + show(powers);
        toolkit::XorBasis basis;
        uint64_t reachable = 0;
        for (auto value : powers) {
            size_t rank = basis.rank();
            require(basis.insert(value) && basis.rank() == rank + 1, context);
            reachable |= value;
            require(basis.maximum_xor() == reachable, context);
            require(basis.contains(reachable), context);
        }
        require(basis.rank() == 64 && basis.maximum_xor(random64()) == UINT64_MAX, context);
        require(!basis.insert(UINT64_MAX) && basis.contains(random64()), context);
    }
    success();
}
