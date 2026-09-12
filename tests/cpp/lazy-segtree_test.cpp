#include "test_util.hpp"
#define TOOLKIT_EXAMPLE_NO_MAIN
#include "../../examples/lazy-segtree.cpp"

// Independent oracle: integer residues, direct affine updates and range sums.
constexpr long long mod_value = 998244353;
long long residue(long long x) { return (x % mod_value + mod_value) % mod_value; }
long long direct_sum(const vector<long long>& values, int l, int r) {
    long long sum = 0;
    for (int i = l; i < r; ++i) sum = (sum + values[i]) % mod_value;
    return sum;
}
void check_range(AffineTree& tree, const vector<long long>& values,
                 int l, int r, const string& input) {
    Segment actual = tree.prod(l, r);
    require(actual.sum.val() == direct_sum(values, l, r) && actual.length == r - l,
            input + " prod=" + to_string(l) + "," + to_string(r));
}
void update(AffineTree& tree, vector<long long>& values, int l, int r,
            long long a, long long b, string& input) {
    input += ";apply(" + to_string(l) + "," + to_string(r) + "," +
             to_string(a) + "," + to_string(b) + ")";
    tree.apply(l, r, Affine{a, b});
    for (int i = l; i < r; ++i) values[i] = (residue(a) * values[i] + residue(b)) % mod_value;
}
void run_case(vector<long long> values) {
    ++cases_checked;
    string input = "initial=" + show(values);
    vector<Segment> leaves;
    for (long long& x : values) {
        x = residue(x);
        leaves.push_back({x, 1});
    }
    int n = int(values.size());
    AffineTree tree(leaves);
    // Leave noncommuting tags pending before any query pushes them.
    update(tree, values, 0, n, 2, 3, input);
    update(tree, values, 0, n, 5, 7, input);
    for (int step = 0; step < 70; ++step) {
        int kind = int(rng() % 5);
        int l = int(rng() % (n + 1)), r = int(rng() % (n + 1));
        if (l > r) swap(l, r);
        long long a = int(rng() % 11) - 5, b = int(rng() % 17) - 8;
        if (kind <= 1) update(tree, values, l, r, a, b, input);
        else if (kind == 2 && n) {
            int p = int(rng() % n);
            input += ";apply_point(" + to_string(p) + "," + to_string(a) + "," + to_string(b) + ")";
            tree.apply(p, Affine{a, b});
            values[p] = (residue(a) * values[p] + residue(b)) % mod_value;
        } else if (kind == 3 && n) {
            int p = int(rng() % n);
            input += ";set(" + to_string(p) + "," + to_string(b) + ",length=1)";
            tree.set(p, Segment{b, 1});
            values[p] = residue(b);
        } else check_range(tree, values, l, r, input);
        Segment total = tree.all_prod();
        require(total.sum.val() == direct_sum(values, 0, n) && total.length == n,
                input + " all_prod");
        if (step % 7 == 0) {
            if (n) {
                int p = int(rng() % n);
                Segment point = tree.get(p);
                require(point.sum.val() == values[p] && point.length == 1,
                        input + " get=" + to_string(p));
            }
            int limit = int(rng() % (n + 2)), start = int(rng() % (n + 1));
            auto fits = [limit](Segment x) { return x.length <= limit; };
            string query = input + " boundary start=" + to_string(start) + " length_limit=" + to_string(limit);
            require(tree.max_right(start, fits) == min(n, start + limit), query + " max_right");
            require(tree.min_left(start, fits) == max(0, start - limit), query + " min_left");
        }
        // Check the actual example action laws against plain integer arithmetic.
        long long x = rng() % mod_value, c = rng() % mod_value, d = rng() % mod_value;
        Affine composed = composition(Affine{a, b}, Affine{c, d});
        long long expected = (residue(a) * ((c * x + d) % mod_value) + residue(b)) % mod_value;
        string law = " a=" + to_string(a) + " b=" + to_string(b) + " c=" +
                     to_string(c) + " d=" + to_string(d) + " x=" + to_string(x);
        Segment mapped = mapping(composed, Segment{x, 1});
        require(mapped.sum.val() == expected && mapped.length == 1, input + " composition law" + law);
        Segment unchanged = mapping(identity_map(), Segment{x, 1});
        require(unchanged.sum.val() == x && unchanged.length == 1, input + " identity law" + law);
        require(mapping(Affine{a, b}, empty_segment()).sum.val() == 0,
                input + " empty mapping" + law);
    }
    for (int l = 0; l <= n; ++l)
        for (int r = l; r <= n; ++r) check_range(tree, values, l, r, input);
}
int main() {
    run_case({});
    run_case({0});
    run_case({-1, mod_value, mod_value + 1, -mod_value - 1, 7});
    for (int i = 0; i < 260; ++i) {
        vector<long long> values(rng() % 20);
        for (auto& x : values) x = int(rng() % 101) - 50;
        run_case(values);
    }
    success();
}
