#include "toolkit/rolling_hash.hpp"
#include "test_util.hpp"

string bytes(const string& s) {
    vector<int> out;
    for (unsigned char c : s) out.push_back(c);
    return show(out);
}
// Independent doubling arithmetic, without the implementation's wide multiply.
uint64_t product(uint64_t a, uint64_t b) {
    const uint64_t mod = toolkit::RollingHashContext::modulus;
    uint64_t result = 0;
    while (b) {
        if (b & 1) result = (result + a) % mod;
        a = (a + a) % mod;
        b >>= 1;
    }
    return result;
}
uint64_t direct(const string& s, size_t l, size_t r, uint64_t base) {
    uint64_t value = 0;
    for (size_t i = l; i < r; ++i)
        value = (product(value, base) + (unsigned char)s[i] + 1) % toolkit::RollingHashContext::modulus;
    return value;
}
void check(const string& a, const string& b, uint64_t seed) {
    ++cases_checked;
    auto context = make_shared<const toolkit::RollingHashContext>(seed);
    toolkit::RollingHash ha(a, context), hb(b, context);
    string input = "a=" + bytes(a) + " b=" + bytes(b) + " base_seed=" + to_string(seed) +
                   " base=" + to_string(context->base());
    require(context->base() >= 257 && context->base() <= toolkit::RollingHashContext::modulus - 2,
            input + " allowed base range");
    require(toolkit::RollingHashContext(seed).base() == context->base(), input + " reproducible base");
    for (size_t l = 0; l <= a.size(); ++l) for (size_t r = l; r <= a.size(); ++r) {
        auto actual = ha.slice(l, r);
        auto expected = direct(a, l, r, context->base());
        string range = input + " range=[" + to_string(l) + "," + to_string(r) + ")";
        require(actual.value == expected && actual.length == r - l, range);
        toolkit::RollingHash copied(a.substr(l, r - l), context);
        require(ha.possibly_equal(l, r, copied, 0, copied.size()), range + " normalized substring");
    }
    for (int q = 0; q < 40; ++q) {
        size_t l = rng() % (a.size() + 1), r = rng() % (a.size() + 1);
        size_t u = rng() % (b.size() + 1), v = rng() % (b.size() + 1);
        if (l > r) swap(l, r);
        if (u > v) swap(u, v);
        bool candidate = ha.possibly_equal(l, r, hb, u, v);
        bool reference = r - l == v - u && direct(a, l, r, context->base()) == direct(b, u, v, context->base());
        string ranges = input + " a=[" + to_string(l) + "," + to_string(r) + ") b=[" +
                        to_string(u) + "," + to_string(v) + ")";
        require(candidate == reference, ranges + " direct fingerprint comparison");
        if (a.substr(l, r - l) == b.substr(u, v - u)) require(candidate, ranges + " no false negative");
        // Unequal strings are permitted to collide: do not turn this into an equality oracle.
    }
    context.reset();
    auto copied = ha;
    require(copied.slice(0, a.size()) == ha.slice(0, a.size()), input + " owned context lifetime");
}
int main() {
    check("", "", 0);
    check("banana", "ananas", 1729);
    check(string("\0\xff\0\xff", 4), string("\xff\0", 2), UINT64_MAX);
    check("aaaaaaaaaaaaaaaa", "aaaa", 1);
    for (int t = 0; t < 400; ++t) {
        string a(rng() % 19, '\0'), b(rng() % 19, '\0');
        for (char& c : a) c = char(rng() % (t % 2 ? 4 : 256));
        for (char& c : b) c = char(rng() % (t % 2 ? 4 : 256));
        uint64_t seed = (uint64_t(rng()) << 32) | rng();
        check(a, b, seed);
    }
    auto a = make_shared<const toolkit::RollingHashContext>(7);
    auto b = make_shared<const toolkit::RollingHashContext>(7);
    toolkit::RollingHash h("abc", a), other("abc", b);
    bool threw = false;
    try { (void)h.possibly_equal(0, 3, other, 0, 3); } catch (const invalid_argument&) { threw = true; }
    require(threw, "same seed but different context identity must reject comparison");
    for (auto [l, r] : vector<pair<size_t, size_t>>{{2, 1}, {0, 4}, {SIZE_MAX, SIZE_MAX}}) {
        threw = false;
        try { (void)h.slice(l, r); } catch (const out_of_range&) { threw = true; }
        require(threw, "invalid abc range=" + to_string(l) + "," + to_string(r));
    }
    threw = false;
    try { toolkit::RollingHash invalid("", nullptr); } catch (const invalid_argument&) { threw = true; }
    require(threw, "null rolling hash context");
    success();
}
