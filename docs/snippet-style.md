# Compact snippet conventions

Compact code is a presentation choice, not an excuse to remove contracts,
provenance, licenses, validation or dependency information.

## What is actually available

`include/toolkit/base.hpp` contains:

```cpp
#pragma once
#include <bits/stdc++.h>
using namespace std;
```

`include/toolkit/kactl_prelude.hpp` includes `base.hpp` and defines exactly:

```cpp
using ll = long long;
using vi = vector<int>;
using pii = pair<int, int>;
#define sz(x) int((x).size())
#define all(x) begin(x), end(x)
#define rep(i, a, b) for (int i = (a); i < (b); ++i)
```

No other shorthand is promised. In particular, do not assume `pb`, `fi`,
`se`, `rall`, loop variants, debug macros, a global modulus, or a generic
infinity constant. Read a header before relying on its own declarations.
`sz` narrows to `int`, so the represented size must fit. Macro arguments can
be evaluated more than once; do not pass expressions with side effects.
`rep` is a half-open increasing loop with `int` indices.

The original compact helpers use `base.hpp` where appropriate rather than
adding long lists of individual standard headers merely for display. Complete
examples retain necessary includes and `main` so they actually compile.
Notebook **snippet/reference content** is separate from the **complete
standalone example**, and a true dependency appendix retains prerequisites.
Do not remove upstream includes, authors or license comments for aesthetics.

## Structures and algorithms

`struct` and `class` have the same language capabilities; their default
member/base access differs. Choosing `struct` for a small public contest
data structure is convenient, not a runtime speed optimization.

Prefer a clear contract, justified complexity and an independently checked
implementation over hyper-optimization. Fast-math, native CPU pragmas and
AVX2 assumptions are not part of the default core. Short names should still
make the invariant and state transitions understandable.

Collision mitigation applies to `unordered_map` and `unordered_set`.
Capacity reservation/load-factor tuning helps expected performance; a
salted hash mitigates predictable collision attacks but gives no universal
worst-case bound. Ordered `set`, `map`, and `multiset` are tree-based; they
do not need “anti-hash.” Distinguish erasing one multiset iterator from
erasing all occurrences of a value.

These conveniences target GNU GCC/libstdc++ and `bits/stdc++.h`.
They do not promise ISO compiler independence. A judge submission still needs
the full required code with its supported domains and type limits respected.
