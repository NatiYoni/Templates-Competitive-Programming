# Contest workflow

## Before the event

- Confirm the actual ETCPC compiler/flags, memory and stack limits, allowed
  references, printing rules, and team notebook page restrictions. The current
  exact compiler has not been publicly verified here.
- Acquire the locked source shelf while online. Run the 40 baseline local
  integrations, then generate and open the compact/core notebook offline.
- Compile each adapted snippet independently with the contest flags.
  Rehearse the APIs rather than merely carrying more pages.
- Print-preview long code, notices and dependency pages. Do not assume the
  browser's page breaks or a particular upstream notebook's page limit.

## Choose the algorithm before optimizing it

Estimate the input size and operation count. Quadratic work at 200,000 items
is not rescued by a macro; a small exponential state space can be simpler and
safer than a sophisticated data structure. Account for memory constants,
per-edge allocations, recursion depth and the cost of repeated initialization.
Write down the invariant or greedy exchange argument. Identify whether data
is static/dynamic, online/offline, weighted/unweighted, or adversarial.

## Input, state, and numeric discipline

- Decide zero/one-based indexing and closed/half-open ranges explicitly.
  Check empty input, singleton ranges, disconnected graphs and duplicates.
- Choose sentinels outside valid values without making arithmetic overflow.
  Test reachability before adding to an infinity sentinel. Widen operands
  before multiplication; signed overflow is undefined behavior.
- Document invalid weights/alphabet/moduli and reject or avoid unsupported
  inputs. Do not silently clamp invalid input into a different problem.
- Clear per-case arrays, residual graphs, automata, caches and global state.
  Treat rollback snapshots and dynamic modulus changes carefully.
- Prefer iterative traversal when an adversarial chain may exhaust the
  stack. A helper passing small tests is not evidence for unlimited recursion.
- Use `ios::sync_with_stdio(false); cin.tie(nullptr);` when appropriate and
  do not carelessly mix unsynchronized C and C++ I/O. Flush interactive output
  when required; ordinary batch problems rarely need `endl` on every line.

## Stress testing

Start with a tiny independent brute-force oracle, not a second copy of the
same optimized recurrence. Enumerate or use a reproducible seed; record the
failing input and reduce it. Include emptiness, ties, repeated values, cycles,
parallel/self edges, extreme supported values, singular matrices and
collinear points where the contract allows them.

Use tolerances tied to scale for numerical checks; exact equality is not
appropriate for approximate solvers. Sanitize representative cases when the
toolchain supports it. Always rerun after changing source, helpers, tests or
contracts—historical evidence is not current evidence.

## Common STL traps

- Comparators must implement strict weak ordering. `<=` is not an ordering
  predicate for `sort`/ordered containers. Do not mutate comparison keys
  while they remain stored.
- `lower_bound` requires the matching sorted order; `unique` alone does not
  globally deduplicate. Distinguish stable sorting requirements.
- Vector growth invalidates references/iterators; erasing while iterating
  needs the returned iterator and the container's exact invalidation rules.
- `map[key]` and `unordered_map[key]` insert missing keys. `priority_queue`
  is a max-heap by default. A queue, stack and deque expose different access
  operations and complexity.
- `multiset.erase(value)` erases all equivalent values; erase a found
  iterator to remove one. `set`, `map` and `multiset` are ordered tree-based
  containers, not hash tables.
- `unordered_map`/`unordered_set` can suffer adversarial collisions.
  Reserve capacity and manage load factor for ordinary efficiency; a salted
  good-quality hash can mitigate predictable attacks but cannot give a
  universal worst-case guarantee. Use an ordered container when guaranteed
  logarithmic operations matter. “Anti-hash” does not apply to ordered
  `multiset`.
- Check signed/unsigned comparisons, negative remainder normalization,
  bit-shift widths, and zero arguments to count-leading/trailing-zero builtins.
  `__builtin_popcount` counts `unsigned int`; use a width-matched operation.

## Before submitting

1. Restate the invariant, complexity and supported domain.
2. Check indices, overflow, sentinels, unreachable/cycle/inconsistent states.
3. Check initialization, repeated cases, global state and recursion depth.
4. Run hand cases, boundaries and a small independent stress oracle.
5. Remove diagnostic output; retain required I/O and output formatting.
6. Compile with the actual judge flags and verify the submitted file contains
   every required dependency. A local include path is not a judge submission.
7. Review the problem's exact tie-breaking, precision and empty-case rules.
