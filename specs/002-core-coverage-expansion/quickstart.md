# Expanded Toolkit Entry Points

Use this dedicated worktree, never the protected main checkout or another
repository. Requirements remain Git, Python3.10+, and GCC supporting the
documented GNU C++17 baseline. Only online `sync` needs the network.

```sh
python3 tools/toolkit.py doctor
python3 tools/toolkit.py sync --all
python3 tools/toolkit.py sync --all --offline
python3 tools/toolkit.py validate --sources --catalog
python3 tools/toolkit.py test --selection-profile notebook/profiles/expanded.json --seed 1729 --record
python3 tools/toolkit.py index
python3 tools/toolkit.py notebook --profile notebook/profiles/compact.json
python3 tools/toolkit.py notebook --profile notebook/profiles/core.json --output-dir build/core
python3 tools/toolkit.py notebook --profile notebook/profiles/expansion.json --output-dir build/expansion
python3 tools/toolkit.py notebook --profile notebook/profiles/expanded.json --output-dir build/expanded
python3 tools/toolkit.py validate --sources --catalog --links
```

`expanded.json` selects76 entries. `expansion.json` selects just the36 new
integrations. `--all-core` and `core.json` still select the original40; the
original compact12 selection is unchanged. Compiler `--profile` continues
to select baseline/sanitizers, independently of `--selection-profile`.

Open `build/index.html` or any generated `notebook.html` directly via `file://`.
No web server or downloaded assets are required. Markdown alternatives are
generated alongside them. Browser Print is optional and page counts depend
on full dependencies, contracts, notices and print settings.

Original educational sections appear as reference-only, with local authorship
and content identity rather than an invented source pin. Including them in a
custom notebook requires explicit `include_study_references: true`; that does
not authorize missing or copy-blocked references.

For fresh reproduction before a commit is authorized, export the intended
working-tree files into an isolated directory with no preexisting source or
build products. Acquire all nine pins there and repeat the commands. Identify
this as a fresh exported-working-tree installation, not committed-HEAD release
reproduction. Retain the source/evidence/artifact observations and remove only
that explicitly identified disposable fixture.
