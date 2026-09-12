# Sources, provenance, and permissions

The authoritative acquisition/provenance record is
[`sources/lock.json`](../sources/lock.json). Acquisition means an unmodified
checkout of a particular revision, **not** successful integration or local
algorithm validation. The catalog records specific file references; notebook
generation separately checks permission for each copied dependency.

## Nine approved pins

These full revisions are intentional; commands never replace them with branch
tips. Repository URLs and identity/license evidence are in the lock and the
[research record](../specs/001-comprehensive-icpc-toolkit/research.md).
Observed allocated checkout sizes include Git metadata and are only local
measurements, not download sizes, limits, or future guarantees. The table totals
67,588 KiB (about 66 MiB), including the shallow Git histories; filesystem
allocation and reporting units affect the approximate “67 MB” observation.

| Source | Author / classification | Full revision | Local KiB |
|---|---|---|---:|
| [kactl](https://github.com/kth-competitive-programming/kactl) | KTH KACTL contributors; ICPC notebook | `fb67e471a878eb06ad9ed036c38c76b6c913847c` | 13,316 |
| [acl](https://github.com/atcoder/ac-library) | AtCoder; official reusable library | `864245a00b00dd008d1abfdc239618fdb7d139da` | 5,052 |
| [cp-algorithms](https://github.com/cp-algorithms/cp-algorithms) | CP-Algorithms contributors; educational articles | `c18a62d01ab9fd816a8cdeedb40cf92e47a60bb1` | 6,480 |
| [cp-algorithms-aux](https://github.com/cp-algorithms/cp-algorithms-aux) | CP-Algorithms contributors; associated implementation library | `2da939345631f2ed51d546b3245e03394352b9c2` | 7,816 |
| [nyaan](https://github.com/NyaanNyaan/library) | Nyaan; reusable library | `b3981adc80a800b2584980b01821324ea6c77183` | 6,388 |
| [luzhiled](https://github.com/ei1333/library) | Luzhiled / ei1333; reusable library | `dca071d33d8dbfd001e450a109c55eb6c38c2ee8` | 6,388 |
| [suisen](https://github.com/suisen-cp/cp-library-cpp) | suisen-cp; reusable library | `c49e847ae29ed11510ad9cd7713f5ec77ddf20b1` | 10,212 |
| [benq](https://github.com/bqi343/cp-notebook) | Benjamin Qi (Benq); mixed notebook/solution archive | `292c614b7b83c355b188234966b3b336bc009f16` | 10,460 |
| [ecnerwala](https://github.com/ecnerwala/cp-book) | Andrew He; reusable library | `6d94fa04f139e11e71352fecbdc8de241fa038c3` | 1,476 |

Benjamin Qi's matching GitHub/Codeforces real-name evidence supports the Benq
attribution; Andrew He's matching evidence supports **ecnerwala** (not the
supplied misspelling “encrwala”). Ownership identity and individual code
authorship are not the same: retain contributor/file-level notices.

## License boundaries

- **KACTL:** its README explicitly describes mixed/unclear licensing.
  There is no blanket clearance. Selected core headers carry explicit CC0
  declarations, except `ConvexHull.h` (Unlicense). `Point.h`, RMQ and every
  transitive copied dependency are checked too. Unlicensed `Manacher.h`,
  `TopoSort.h`, and its contest template are not copied into the core.
  Some broader KACTL references remain copy-blocked and readable only by link.
- **ACL:** root `LICENSE` is CC0-1.0. Core algorithm headers use this grant;
  documentation libraries have distinct terms in
  `document_en/lib/LICENSE.md`. A code integration does not inherit coverage
  for every upstream header.
- **CP-Algorithms articles and auxiliary:** root licenses are CC-BY-SA-4.0.
  Preserve attribution and share-alike requirements for copied/adapted
  content. They are distinct repositories; the auxiliary association is
  evidenced in the article project's README. Article Markdown is readable
  offline but is **not a ready-to-use snippet** or a recreated MkDocs site.
  The auxiliary verification configuration uses C++23.
- **Nyaan:** root CC0-1.0; review file-specific notices. The README describes
  GCC/C++17 and explicitly warns that some modules require AVX2. Cloning does
  not establish C++17 portability for every file.
- **Luzhiled:** root Unlicense; preserve individual dependency notices and
  inspect module-specific assumptions before adapting.
- **Suisen:** CC0 is explicitly declared in the README's LICENSE section;
  no root LICENSE file is assumed. Some headers require ACL. The uninitialized
  `ac-library` gitlink differs from this toolkit's independent ACL pin.
- **Benq:** root CC0 does not erase inherited-file ambiguity described in
  `Implementations/README.md`. Catalog references are selected from
  `Implementations/content/`, not `Contests/`. Unresolved inherited terms
  block notebook copying even though raw source reading links remain usable.
  The expansion found a concrete minimum-cost circulation reference at
  `Implementations/content/graphs (12)/Flows (12.3)/CapacityScaling.h`.
  It says `Source: Own`, but lacks a file-level license marker accepted by the
  current selective-copy policy. It remains reference-only and copy-blocked;
  no permissive override was invented to remove a coverage gap.
- **ecnerwala:** root CC0 unless otherwise noted; `third_party/sais-lite-2.4.1/COPYING`
  has separate MIT-style terms. Current README requires C++23 and some modules
  use GNU PBDS. Third-party content needs explicit policy review before copying.

The notebook retains full selected source comments and authors/license
notices, and includes available root license texts where appropriate.
Unknown licenses never become permitted merely because a URL is public.
No repository-wide license has been selected for this project's original code;
local original helpers are identified separately, not assigned an invented
upstream license or source revision.
The expansion does not acquire or modify any additional source repository.
Its23 original educational references separately declare
`provenance: repository-original`, local authorship and derived content hashes.
They are neither foreign acquisitions nor a new public license grant. All43
old gap decisions are retained in the [audit](gap-audit.md).

## Named sources not acquired

| Name | Separate findings | Decision |
|---|---|---|
| tourist / Gennady Korotkevich | `the-tourist/algo` exists; observed candidate pin `27958d4a249b07f433cba601a4eee4d877f4caf1`. Strict official attribution was not verified and no explicit LICENSE/COPYING was found. Some observed code uses C++20 `bit_floor`. | Disabled candidate. Do not call it officially verified, copy it, or substitute another archive. |
| jiangly / Lingyu Jiang | Codeforces identity is known, but no officially attributable reusable GitHub library was verified. A forked training-solution repository is not an equivalent reusable notebook. | Disabled. [Codeforces submissions](https://codeforces.com/submissions/jiangly) are an external study link, not copying permission. |
| ksan | User explicitly requested exclusion. | Not an acquisition candidate and not a blocker. |

“Not verified” is a bounded research result, not proof that no official source
can exist. Repository availability, identity evidence, reuse permissions and
the decision to acquire remain separate fields.

## Reproduction

`sync` acquires content-addressed detached checkouts under
`upstream/<id>/<full-sha>/`, verifies origin/HEAD/content, and preserves notices.
Existing modified, untracked, ignored-generated, conflicting or wrong-origin
trees are rejected without reset/deletion. Upstream setup/tests/builds and
recursive submodule acquisition are never run implicitly.

The five gitlinks—ACL's benchmark and gtest, CP-Algorithms' plugin, auxiliary
blazingio, and Suisen's ACL—are explicitly recorded and left uninitialized. The toolkit's
local runner does not require those upstream verification frameworks.
After initial acquisition, `sync --all --offline` verifies the same sources
without fetching. Review pins/licenses/contracts and rerun affected tests
before recording evidence for a deliberate source update.
