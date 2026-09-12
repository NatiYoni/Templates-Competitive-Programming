# Expansion Research and Gap Decisions

The original43 missing IDs were read from the preserved v1 catalog, not inferred
from category labels. Source searches used only the nine existing exact pins;
no new identity hunt or revision update was performed.

## Audit Dispositions

The selected resolutions are seven new integrations, eight pinned references,
23 original educational references, and five residual gaps. After all36
integrations were merged, actual live counts are **76 implemented,
184 reference-only, five missing**, still265 entries. Implementation status
does not substitute for current passing evidence.

Pinned reference decisions were checked by reading the actual files:

| Target | Source and pinned-relative path | Observation |
|--------|---------------------------------|-------------|
| binary-heap | cp-algorithms `src/graph/dijkstra_sparse.md` | Heap-backed priority_queue and lazy deletion are explained in context. |
| reconstruction | cp-algorithms `src/dynamic_programming/longest_increasing_subsequence.md` | Contains predecessor/recovery explanation; the old `src/sequences/` path is only a redirect and was not credited. |
| randomized-incremental-geometry | cp-algorithms `src/geometry/enclosing-circle.md` | Welzl construction, invariants and backward expected-time analysis are substantive; newer-standard pseudocode stays reference-only. |
| amortized-analysis | cp-algorithms `src/data_structures/stack_queue_modification.md` | Explicit aggregate push/transfer/pop analysis for minimum/two-stack queues. |
| prefix-transformations | luzhiled `docs/cumulative-sum.md` | Build/fold lifecycle, half-open queries and complexity. |
| reductions | cp-algorithms `src/graph/flow_with_demands.md` | Saturation/max-flow reduction with feasibility explanation and recovery. |
| divide-and-conquer-on-time | cp-algorithms `src/data_structures/deleting_in_log_n.md` | Active intervals, time segment tree, rollback and amortization warning. |
| min-cost-circulation | benq `Implementations/content/graphs (12)/Flows (12.3)/CapacityScaling.h` | Actual circulation code exists; retain a copy-blocked reference under the current selective-per-file policy. |

The circulation header says `Source: Own`, and the repository has root CC0,
but the existing source policy requires file-level permission review rather
than treating that root as blanket clearance. This audit does not add a new
permission override. `NetworkSimplex.h` cites another source and `MCMF.h` cites
multiple inherited sources; neither is silently copied as a tested substitute.
Missing source and unresolved copying permission are separate states.

## Original Educational Material

Six notes under `docs/techniques/` contain23 individually linked worked sections
covering ordering/container semantics; two-pointers, events, meet-in-the-middle,
recursion/backtracking and subset-state design; proof/workflow patterns;
floating-point/shuffle reasoning; coordinate/difference transforms; and
Johnson reweighting/recovery. Each section states an actual invariant, example
or procedure and limitations. These references are original prose, not
upstream acquisitions or generic tested algorithms.

## Retained Gaps

- `mo-with-updates`: inspected KACTL/Benq/Nyaan static Mo and Luzhiled rollback
  Mo; neither is the temporal point-update variant.
- `minimum-cycle-basis`: an arbitrary independent cycle basis is not a minimum
  weight cycle basis; no suitable concrete reference was verified.
- `randomized-polynomial-identity-testing`: generic polynomial/string-hash
  material lacks the required field/degree/sampling identity-test contract.
- `multidimensional-dominance`: range containers alone were not credited as
  a worked dominance technique with tie/group/dimension semantics.
- `cdq-divide-and-conquer`: time-segment-tree rollback and partition DP are
  different algorithms, not substitute CDQ evidence.

These statements describe this bounded audit of the current pins, not claims
that no such public implementations exist. The exact original-to-final mapping
is retained in [`catalog/gap-audit.json`](../../catalog/gap-audit.json), with
its navigable [per-gap table](../../docs/gap-audit.md).

## Integration Convergence

All76 current baseline records pass:159,783 cases, seed1729, including132,149
from the new36. The expanded Python suite passes101 tests. A convergence
regression caught historical topic names disappearing after more precisely
scoped integration names were adopted. Original names/aliases are now retained
as search aliases, and inventory tests accept canonical names or those aliases
without changing historical feature001 coverage.

An independently acquired fresh installation repeated all76 and produced
matching fingerprints and19 byte-identical generated files. Original compact
and core notebook bytes remain unchanged from v1. Real offline browser and
print observations are recorded in the
[expansion delivery checkpoint](../../docs/testing.md#expansion-delivery-checkpoint).
