# Research: Sources, Permissions, and Architecture

**Checked**: 2026-09-12

These are provenance research observations, not algorithm test results. Exact
pins were retrieved from GitHub repository/commit metadata and subsequently
acquired and checked locally. The delivered lock is `sources/lock.json`;
[source documentation](../../docs/sources.md) records checkout observations,
and [local evidence](../../docs/testing.md) records integration outcomes.

## Existing Repository and Spec Kit

The initial worktree was clean at `261197b` and contained only a 25-byte
`README.md` headed `Competitive-Programming`. No instruction files, license,
Spec Kit setup, build conventions, or tests existed. The branch was renamed
through the app to `natiyoni-comprehensive-icpc-toolkit` before file creation.
No main-checkout or `Sydekse/suqbot` files were accessed.

Official workflow: <https://github.com/github/spec-kit>

Installed Specify CLI 1.0.1 supplied bundled templates/scripts and Copilot
skills via `specify init --here --force --non-interactive --integration copilot
--script sh --ignore-agent-tools`. Its release commit is
`9118ed15a0ba65053469a94c560ea5d233f75884`. Official feature/plan setup scripts
created `001-comprehensive-icpc-toolkit` independently of the app Git branch.
No extra Git extension, branch creation, or commit was invoked.

## ETCPC Baseline Decision

The user subsequently approved implementation with a **C++17-compatible core**
for ETCPC. A current ETCPC compiler version/standard could not be verified from
the [ICPC listing](https://icpc.global/regionals/finder/EtCPC-2026) or
[EtCPC community announcements](https://t.me/s/etcpc). Do not infer regional
settings from World Finals.

The [2026 World Finals environment](https://image.icpc.global/icpc2026/environment.html)
currently lists g++ 13.2.0 and `-std=gnu++20`, explicitly subject to change.
This is context, not confirmation of ETCPC support. Local validation currently
uses GCC 15.2.0 in C++17 mode, not an identical ETCPC judge image. C++23-only
references remain separate. The refined compact-snippet style is adopted
without removing contract, dependency, author, or license information.

The planning-mode file guard initially required staging completed drafts in
session artifacts. After a planning-documents-only approval, the drafts were
materialized in this repository using `apply_patch`. That initial approval did
not authorize toolkit implementation. The later explicit C++17-core approval
did; implementation is complete and the README now contains actual entry points.

## Approved Source Shelf

| ID | Repository and classification | Exact candidate pin | License observation and integration constraints |
|----|-------------------------------|---------------------|------------------------------------------------|
| kactl | [KTH KACTL](https://github.com/kth-competitive-programming/kactl), reusable ICPC notebook | `fb67e471a878eb06ad9ed036c38c76b6c913847c` | README explicitly describes mixed/unclear licensing; no blanket license. Inspect each selected file and dependency. Macros, aliases, globals, and terse contracts need adapters. |
| acl | [AtCoder Library](https://github.com/atcoder/ac-library), official reusable library | `864245a00b00dd008d1abfdc239618fdb7d139da` | Root `LICENSE`: CC0-1.0; documentation libraries have separate notices under `document_en/lib/LICENSE.md`. Core algorithm headers are the preferred stable base. |
| cp-algorithms | [CP-Algorithms articles](https://github.com/cp-algorithms/cp-algorithms), educational source | `c18a62d01ab9fd816a8cdeedb40cf92e47a60bb1` | Root `LICENSE`: CC-BY-SA-4.0. Preserve attribution/share-alike for copied/adapted material. Not a drop-in template library. Article Markdown is available offline without rebuilding its website. |
| cp-algorithms-aux | [CP-Algorithms auxiliary library](https://github.com/cp-algorithms/cp-algorithms-aux), associated implementation library | `2da939345631f2ed51d546b3245e03394352b9c2` | Root `LICENSE`: CC-BY-SA-4.0. `.verify-helper/config.toml` uses `-std=c++23`; do not claim C++17 compatibility by repository association. |
| nyaan | [Nyaan's Library](https://github.com/NyaanNyaan/library), reusable library | `b3981adc80a800b2584980b01821324ea6c77183` | Root `LICENSE`: CC0-1.0. README expects GCC/C++17 and explicitly warns some files require AVX2. Keep those outside default integrations. |
| luzhiled | [Luzhiled / ei1333 library](https://github.com/ei1333/library), reusable library | `dca071d33d8dbfd001e450a109c55eb6c38c2ee8` | Root `LICENSE`: Unlicense. Review individual dependencies/notes; do not infer local tested status from upstream verification. |
| suisen | [Suisen library](https://github.com/suisen-cp/cp-library-cpp), reusable library | `c49e847ae29ed11510ad9cd7713f5ec77ddf20b1` | README `LICENSE` section declares CC0, although no root LICENSE was found and GitHub metadata gives no SPDX result. README explicitly says some headers require ACL. |
| benq | [Benjamin Qi / Benq notebook](https://github.com/bqi343/cp-notebook), mixed notebook and solution archive | `292c614b7b83c355b188234966b3b336bc009f16` | Root `LICENSE`: CC0-1.0, but `Implementations/README.md` warns about inherited per-file licensing. Curate only `Implementations/content/`, not `Contests/`. GNU template/macros and stale paths require care. |
| ecnerwala | [Andrew He's cp-book](https://github.com/ecnerwala/cp-book), reusable library | `6d94fa04f139e11e71352fecbdc8de241fa038c3` | Root `LICENSE`: CC0-1.0 unless otherwise noted. `third_party/sais-lite-2.4.1/COPYING` has separate MIT-style terms. Current README requires C++23; some headers use GNU PBDS. |

All nine repositories were public/not archived at observation. They are
approved acquisition candidates, not already-cloned or locally validated
sources. Do not derive permissions solely from the API's license label.

### Specific Evidence

- [KACTL README at the pin](https://github.com/kth-competitive-programming/kactl/blob/fb67e471a878eb06ad9ed036c38c76b6c913847c/README.md):
  mixed licensing; a 25-page upstream notebook is KACTL's choice, not an ICPC
  rule for this project. It intentionally omits some common algorithms.
- [ACL README](https://github.com/atcoder/ac-library/blob/864245a00b00dd008d1abfdc239618fdb7d139da/README.md):
  official attribution and documentation-library exception.
- [CP-Algorithms README](https://github.com/cp-algorithms/cp-algorithms/blob/c18a62d01ab9fd816a8cdeedb40cf92e47a60bb1/README.md):
  explicitly links the auxiliary competitive-programming library.
- [Auxiliary compile configuration](https://github.com/cp-algorithms/cp-algorithms-aux/blob/2da939345631f2ed51d546b3245e03394352b9c2/.verify-helper/config.toml):
  C++23, warnings, and optimization settings.
- [Nyaan README](https://github.com/NyaanNyaan/library/blob/b3981adc80a800b2584980b01821324ea6c77183/README.md):
  C++17/GCC and AVX2 caveat.
- [Suisen README](https://github.com/suisen-cp/cp-library-cpp/blob/c49e847ae29ed11510ad9cd7713f5ec77ddf20b1/README.md):
  direct CC0 declaration and ACL dependency.
- [Benq GitHub identity](https://api.github.com/users/bqi343) and
  [Codeforces identity](https://codeforces.com/profile/Benq) both name Benjamin Qi.
  [Notebook README](https://github.com/bqi343/cp-notebook/blob/292c614b7b83c355b188234966b3b336bc009f16/Implementations/README.md)
  identifies sources and explains inherited licensing uncertainty.
- [ecnerwala GitHub identity](https://api.github.com/users/ecnerwala) and
  [Codeforces identity](https://codeforces.com/profile/ecnerwala) both name
  **Andrew He**. Correct the user's likely misspelling `encrwala` to `ecnerwala`.
  [Library README](https://github.com/ecnerwala/cp-book/blob/6d94fa04f139e11e71352fecbdc8de241fa038c3/README.md)
  identifies it as his reference library and requires C++23.

### Named Sources Not Enabled

| Name | Finding | Decision |
|------|---------|----------|
| tourist / Gennady Korotkevich | [the-tourist/algo](https://github.com/the-tourist/algo) exists and contains reusable algorithms. Observed pin `27958d4a249b07f433cba601a4eee4d877f4caf1`. Account metadata lacks real-name/website evidence; `author: tourist` in a template is insufficient for the requested strict attribution. No LICENSE/COPYING was found. Some current code uses C++20 `bit_floor`. | Disabled candidate with separate attribution and permission blockers; do not call it a verified official source or a contest archive. |
| jiangly / Lingyu Jiang | [Codeforces profile](https://codeforces.com/profile/jiangly) identifies Lingyu Jiang. No officially attributable reusable GitHub library was verified. `jiangly-programmer/ioi2021-homework` is a forked training-solution repository, not a verified reusable notebook; account attribution and licensing also remain unresolved. | Record "no verified official reusable repository found"; retain [Codeforces submissions](https://codeforces.com/submissions/jiangly) as an external study link. No substitute clone. |
| ksan | The user explicitly chose to skip the unresolved identity. | Excluded, not a blocker or an invitation to search similarly named accounts. |

Negative findings mean "not verified in this research", not proof that an
official library cannot exist. No direct Codeforces-to-GitHub link was found
for the four competitor profiles; Benq/ecnerwala satisfy the alternative
explicit matching-real-name evidence route.

## Selected KACTL Licensing and Dependency Findings

The planned core uses explicitly marked CC0 files:
`RMQ.h`, `UnionFindRollback.h`, `LCA.h`, `HopcroftKarp.h`, `KMP.h`,
`AhoCorasick.h`, `LIS.h`, `Eratosthenes.h`, `SolveLinear.h`, `Point.h`,
`PolygonArea.h`, and `Integrate.h`. `ConvexHull.h` explicitly says Unlicense.
Exact paths are listed in coverage. File headers were inspected at the pin.

`LCA.h` includes `../data-structures/RMQ.h`; geometry uses `Point.h`.
No blanket copying of `content/contest/template.cpp` is planned.
`Manacher.h` and `TopoSort.h` lacked explicit license markers and are excluded
from copied core content. Use licensed alternate references for Manacher and
an original elementary Kahn topological-sort example.

HLD is reference-only initially: its KACTL dependency chain includes
`LazySegmentTree.h` and `BumpAllocator.h`, with state/capacity and allocation
considerations. Do not infer integration readiness from the HLD header alone.

## Architecture Decisions

### Pinned Clones, Not Giant Vendoring

**Decision**: Ignore content-addressed local clones and build artifacts; commit
reviewable lock/catalog/adapter/test data.

**Rationale**: Reproducible acquisition without bloating Git or overwriting
existing local work when pins change.

**Alternatives considered**: Vendored repositories duplicate history and
obscure ownership. Submodules are legitimate but complicate a broad optional
reference shelf and source-specific evidence. Floating clones are not pinned.

### Minimal C++17 Baseline

**Decision**: Use ACL, clearly licensed selected KACTL headers, and small tested
original fundamentals for the initial 40. Keep other repositories as references.

**Rationale**: Dependability is achievable without certifying thousands of
upstream files or raising the standard because a reference library does.

**Alternatives considered**: Mass header import, running all upstream test
frameworks, or writing every advanced algorithm originally are out of scope.

### Offline HTML/Markdown

**Decision**: Standard-library generation, embedded local search, configurable
selection, and browser printing; raw article Markdown remains readable offline.

**Rationale**: Avoid a TeX/web-framework dependency and arbitrary page rule.

**Alternatives considered**: Rebuilding every upstream site/PDF is heavyweight
and may execute shell-escape/build scripts or need external assets. A giant PDF
without an algorithm catalog does not establish coverage.

### Independent Evidence

**Decision**: Content-bound local reports, separate from upstream claims and
implementation state.

**Rationale**: A source update or test failure must not inherit an old badge.

**Alternatives considered**: Handwritten "tested" tags and importing upstream
badges cannot establish that this repository's actual integrations work.
