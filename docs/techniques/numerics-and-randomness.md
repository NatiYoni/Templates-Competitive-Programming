# Numerical and randomization discipline

Original educational notes by Toolkit contributors. These explanations are
reference-only, not certified numerical or probabilistic solvers. No public
repository-wide license is selected for this original material.

## Numerical stability and epsilon comparisons

Distinguish the mathematical problem's conditioning from the stability of its
implementation. If two nearly equal large values are subtracted, their small
difference may have few meaningful digits even when each input was rounded
accurately. Increasing the precision can help but does not repair an
ill-conditioned model or an invalid algorithmic assumption.

An absolute tolerance is meaningful near zero; a relative tolerance scales
with the result's magnitude. A common comparison combines both, but the chosen
constants must follow the problem's units, input uncertainty and accumulated
operations. “Use epsilon1e-9 everywhere” has no general justification. A dot
product of many terms, a determinant near degeneracy, and a simple coordinate
comparison have different error behavior.

Do not use approximate equivalence as a sorting comparator: it need not be
transitive. Reject or explicitly handle nonfinite values. Prefer exact integer
predicates when their intermediate products fit a proven wide type, and
normalize scales before a linear solve when appropriate. Check the residual
against the original system, not only whether elimination found nonzero pivots.
An apparently small residual still need not imply an accurate solution for an
ill-conditioned system. Keep analytic and degenerate test cases alongside
random checks.

## Shuffle invariants

Fisher-Yates fixes one remaining position at a time by choosing uniformly among
the still-unfixed positions. Conditioned on earlier choices, every remaining
element must have equal chance of occupying the next position; multiplying
these probabilities gives equal probability for each permutation. Swapping
each position with a uniformly chosen position from the entire array is a
different algorithm and is generally biased.

`std::shuffle` with a suitable standard uniform random bit generator avoids
the naive `rng() % k` modulo-bias mistake. A fixed seed is useful for
reproduction, not for unpredictability against an adversary who knows it.
Neither `mt19937` nor ordinary contest shuffling is cryptographic randomness.
The exact permutation is not guaranteed to match across different standard
library implementations even with the same engine seed.

Shuffling changes traversal order, so verify the algorithm's correctness does
not depend on that order. An expected-time bound relies on its stated random
distribution, not on one favorable seed. Distinguish Las Vegas behavior
(correct result, variable running time) from Monte Carlo behavior (possible
incorrect result). Hash equality has collision risk; rerunning the same seed
does not produce independent trials.
