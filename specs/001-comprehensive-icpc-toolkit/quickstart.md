# Quickstart and Acceptance Walkthrough

**Delivery status**: Implemented and exercised in this worktree and a fresh
exported-working-tree installation. These are executable entry points, not
planning placeholders. See [acceptance results](checklists/requirements.md).

## Current Workspace

```text
/home/natnael/copilot-worktrees/Templates-Competitive-Programming/natiyoni-cuddly-journey
```

The app-managed branch is `natiyoni-comprehensive-icpc-toolkit`. Renaming a
branch does not rename the physical worktree directory. Use this worktree,
not the main checkout or `Sydekse/suqbot`.

## Resume Spec Kit

Official scaffolding and completed planning documents are installed. Select
the feature in a new checkout without renaming Git:

```sh
export SPECIFY_FEATURE_DIRECTORY=specs/001-comprehensive-icpc-toolkit
.specify/scripts/bash/check-prerequisites.sh --json --require-tasks --include-tasks
```

`.specify/feature.json` is machine-local ignored state. The actual Git branch
does not need to match Spec Kit's feature-directory basename.

## Baseline Prerequisites

Git, Python 3.10+, GCC with C++17, and a browser for optional HTML/printing.
Initial acquisition needs network/disk capacity. No TeX, CMake, pip packages,
or upstream verification helpers are required for these toolkit commands.

## Full Installation

```sh
python3 tools/toolkit.py doctor
python3 tools/toolkit.py sync --all
python3 tools/toolkit.py validate --sources --catalog
python3 tools/toolkit.py test --all-core --profile baseline --seed 1729 --record
python3 tools/toolkit.py index
python3 tools/toolkit.py notebook --profile notebook/profiles/compact.json
python3 tools/toolkit.py notebook --profile notebook/profiles/core.json --output-dir build/core
python3 tools/toolkit.py validate --links
```

Expected: nine exact source pins, 40 independently compiling and passing core
entries, a truthful broader catalog, and locally linked generated documents.
Open `build/index.html`, `build/notebook/notebook.html` (12 entries), and
`build/core/notebook.html` (40 entries) directly. Markdown versions are also
generated. Print notebook HTML through the browser if a PDF is useful; confirm
the specific contest's page/reference rules separately. Compact refers to the
selection, not a promised page limit: complete dependencies and notices remain.

## Offline Acceptance

Disable network access or instrument subprocess/network calls in the acceptance
harness, then run:

```sh
python3 tools/toolkit.py sync --all --offline
python3 tools/toolkit.py search "centroid decomposition"
python3 tools/toolkit.py test --entry dsu --entry crt
python3 tools/toolkit.py index
python3 tools/toolkit.py notebook --profile notebook/profiles/compact.json
python3 tools/toolkit.py validate --links
```

Expected: no network requests, working `file://` search/navigation, correct
reference-only labels, and no changed upstream checkout content.

## Determinism, Safety, and Fresh-Checkout Scenarios

1. Generate the same index/profile twice and compare content hashes; source/
   metadata/evidence inputs are unchanged and timestamps are excluded.
2. In isolated test fixtures, make an upstream checkout dirty, create a
   wrong-origin destination, provide an escaping path/symlink, simulate a
   failed fetch/concurrent writer, and request an unavailable pin. Each case
   must fail nonzero and preserve pre-existing files.
3. Modify an example or a transitive dependency in an isolated fixture. Old
   passing evidence must become stale; a failed rerun overrides historical pass.
4. Test all statuses and special characters in HTML/search fixtures, including
   paths with spaces and literal closing script tags.
5. Reproduce from a separate clean temporary checkout with no preexisting
   `upstream/` or `build/`. Before implementation files are committed, export
   the intended working-tree inputs instead and identify this honestly as a
   fresh snapshot, not reproduction from committed `HEAD`. Follow only the
   documented commands. Never use the protected main checkout for acceptance.
6. Report actual acquired/integrated/tested counts and every required core
   blocker; do not substitute upstream test badges for local results.
