# Ordering and standard containers

Original educational notes by Toolkit contributors. These are explanations,
not separately tested algorithm integrations. No repository-wide public license
is selected for this original material. Complexity below assumes the standard
operations and a comparator whose own cost is constant.

## Strict weak ordering

`sort`, `set`, and `map` need a strict weak ordering. In particular, `less(x,x)`
must be false; comparison must be transitive; and the equivalence relation
`!less(a,b) && !less(b,a)` must also be transitive. Returning `a <= b` is invalid.
For records, comparing `tie(priority,id)` gives an explicit lexicographic order.
If the comparator ignores `id`, a set can intentionally regard different
records as the same key.

Do not compare integers by subtracting them: the subtraction can overflow
before its sign is inspected. Do not let a stored key's comparison depend on
mutable external state. Epsilon-based floating comparison can make equivalence
nontransitive: at tolerance1, values0,0.75,1.5 demonstrate the problem. Handle
NaNs explicitly and use a genuine ordering for sorting; apply numerical
tolerances afterward. A useful diagnostic enumerates triples and checks these
properties, but finite tests cannot prove an arbitrary comparator correct.

## Stable sorting

Stability preserves the original relative order of equivalent keys. For
records `(group,arrival)`, sorting by group stably keeps arrival order inside
each group. An ordinary sort by `(group,arrival)` achieves that only if arrival
was an explicit unique position; a domain timestamp with ties is not necessarily
the original input order.

To compose keys using stable passes, process the least significant key first,
then increasingly significant keys. Reversing that order usually loses the
desired primary grouping. `stable_sort` still requires strict weak ordering;
stability does not repair a broken comparator. With sufficient auxiliary
memory its comparison complexity is `O(n log n)`; C++17 permits a fallback
with `O(n log^2 n)` comparisons. Budget temporary memory rather than assuming
the in-place behavior of ordinary `sort`. Empty and all-equal inputs are useful
checks for record preservation and tie semantics.

## Queue

A FIFO queue removes the oldest inserted item. In BFS, marking a vertex when
it is enqueued prevents parallel incoming edges from inserting repeated work;
with an unweighted graph, FIFO processing then visits layers in distance order.
Marking only when removing can grow the queue unnecessarily.

With the default deque backing, `push`, `front`, `back`, and `pop` are constant
time. `pop` removes without returning a value: copy `front()` before popping
if the value is needed. Calling `front`, `back`, or `pop` on an empty queue is
invalid. The adapter deliberately exposes no random indexing or iterators.
Using `vector.erase(begin())` as a replacement shifts every remaining item and
can make a linear BFS quadratic. A vector plus a monotonically advancing head
index is another option, but its retained storage is not automatically freed
as items leave.

## Stack

A LIFO stack removes the most recently inserted item. This models nested
unfinished work: for bracket checking, push an opening bracket and require the
next closing bracket to match the current top. Checking only the final counts
would accept `([)]`, which violates nesting.

`stack<T>` exposes `push`, `top`, and `pop`; the last two require a nonempty
stack. A `vector<T>` used with `push_back`, `back`, and `pop_back` additionally
permits inspection, with amortized constant-time pushes. Vector reallocation
can invalidate references to earlier elements. To replace a recursive DFS
exactly, a frame often needs both its vertex and the index of the next edge;
a plain vertex stack does not reproduce recursive exit events. Store enough
state to distinguish entering from finishing a node.

## Deque

A deque supports insertion/removal at both ends and constant-time indexed
access, but does not promise contiguous storage. For 0-1 BFS, a relaxed zero-cost
edge goes to the front and a unit-cost edge to the back; arbitrary nonnegative
weights do not satisfy that scheduling argument.

Removing from either end requires a nonempty deque. Middle insertion/removal
can be linear and is not a cheap substitute for a balanced tree. Do not retain
iterators across mutations without consulting the precise invalidation rules;
index arithmetic is often simpler for a sliding-window algorithm.

A monotone deque stores only candidates, not every logical queue element.
Expire candidates by index and define how equal values are retained. Removing
a dominated candidate is justified only because it can never win while the
newer candidate remains active. This differs from an ordinary FIFO queue whose
entire contents must be preserved.

## Ordered set

`set<T>` stores one representative per comparator-equivalence class and provides
logarithmic lookup, insertion and erasure by key. `lower_bound(x)` finds the
first element not less than `x`; always check for `end()` before dereferencing.
To find a predecessor, first ensure the iterator is not `begin()`.

For duplicate values use `multiset`. `erase(value)` removes every equivalent
key; `erase(iterator)` removes one occurrence. If identity matters, store
`(value,id)` so that two equal values remain distinct. Updating an ordered key
requires erase/reinsert rather than changing comparison-relevant state in place.

These are tree-based containers, not hash tables; an anti-collision hash is
irrelevant. Also, `distance(s.begin(),it)` is linear on set iterators. A set
does not supply logarithmic rank/order-statistic queries merely because lookup
is logarithmic. Coordinate compression plus a Fenwick tree is often sufficient
when all possible keys are known in advance.

## Ordered map

`map<K,V>` associates values with unique comparator-equivalence classes of keys.
`find` and `lower_bound` are logarithmic; map iteration follows key order, not
insertion order. A missing key accessed by `operator[]` is inserted with a
default-initialized value, so a read-looking expression can mutate the map.
Use `find` for optional lookup and `at` when absence should be an error.

For event aggregation, `delta[position] += amount` intentionally uses this
insertion behavior. For a memoized recurrence, however, a default zero can be
indistinguishable from a genuine computed answer. Keep presence separate from
the stored value. Mutating an existing mapped value does not reorder keys.
As with sets, references to erased elements become invalid, and comparator
state must remain stable. Ordered maps have deterministic logarithmic search;
`unordered_map` has different ordering, average-complexity and hash assumptions.
