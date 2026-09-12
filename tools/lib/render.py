"""Deterministic offline HTML/Markdown and permission-checked notebook closure."""
from collections import OrderedDict
import html
from html.parser import HTMLParser
import json
import os
from pathlib import Path
import re
from urllib.parse import quote, unquote, urlsplit

from . import catalog, models

ASSETS = Path(__file__).resolve().parents[1] / "assets"
INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^">]+)[">]', re.MULTILINE)
INCLUDE_DIRECTIVE = re.compile(r"^\s*#\s*include\b", re.MULTILINE)
PERMITTED = {"CC0-1.0", "CC-BY-SA-4.0", "Unlicense", "MIT", "BSD-2-Clause", "BSD-3-Clause"}
GENERATED_MARKER = "toolkit-generated-document"


def _read(path):
    try:
        return Path(path).read_text(encoding="utf-8")
    except (OSError, UnicodeError) as exc:
        raise ValueError(f"Cannot read {path}: {exc}") from exc


def _output(root, output_dir):
    root = Path(root).resolve()
    output = Path(output_dir)
    if not output.is_absolute():
        output = root / output
    output = output.resolve()
    if not output.is_relative_to(root) or output == root:
        raise ValueError(f"Output directory must be inside the toolkit root: {output}")
    if output.relative_to(root).parts[0] in {"upstream", "sources", "catalog", "include", "examples", "tests", "tools", "docs", "notebook", ".git", ".specify", "specs"}:
        raise ValueError(f"Output directory would overwrite source material: {output}")
    return output


def _url(target, document):
    return quote(os.path.relpath(target, Path(document).parent).replace(os.sep, "/"), safe="/")


def _md(text):
    return re.sub(r"([\\`*_\[\]{}()#+.!|>-])", r"\\\1", html.escape(str(text), quote=False))


def _fence(text, language=""):
    runs = [len(run) for run in re.findall(r"`+", text)]
    fence = "`" * max(3, max(runs, default=0) + 1)
    return f"{fence}{language}\n{text.rstrip()}\n{fence}\n"


def _page(title, body, search_data=None):
    data = ""
    if search_data is not None:
        encoded = json.dumps(search_data, ensure_ascii=True, sort_keys=True, separators=(",", ":"))
        encoded = encoded.replace("<", "\\u003c").replace(">", "\\u003e").replace("&", "\\u0026")
        data = f'<script id="search-data" type="application/json">{encoded}</script><script src="search.js" defer></script>'
    return (
        '<!doctype html>\n<html lang="en"><head><meta charset="utf-8">'
        f'<meta name="{GENERATED_MARKER}" content="1">'
        '<meta name="viewport" content="width=device-width,initial-scale=1">'
        f'<title>{html.escape(title)}</title><link rel="stylesheet" href="style.css">'
        f'</head><body><main><h1>{html.escape(title)}</h1>{body}</main>{data}</body></html>\n'
    )


def _write_outputs(root, output, files):
    output.mkdir(parents=True, exist_ok=True)
    for name, content in files.items():
        target = models.safe_path(output, name)
        try:
            target.write_text(content, encoding="utf-8", newline="\n")
        except OSError as exc:
            raise ValueError(f"Cannot write {target}: {exc}") from exc
    errors = validate_links(root, output)
    if errors:
        raise ValueError("Generated link validation failed: " + "; ".join(errors))


def _references(entry, root, document):
    html_links, md_links = [], []
    for ref in entry["locations"]:
        original = ref.get("provenance") == "repository-original"
        label = (_original_label(ref) if original else
                 f"{ref['source']}/{ref['path']} ({ref['kind']}; {ref['license']}; "
                 f"{ref['author']}; pin {ref['revision']})")
        if ref["available"]:
            link = _url(Path(root) / ref["local_path"], document)
            if original and ref.get("fragment"):
                link += "#" + quote(ref["fragment"], safe="")
            html_links.append(f'<a href="{html.escape(link, quote=True)}">{html.escape(label)}</a>')
            md_links.append(f"[{_md(label)}]({link})")
        else:
            action = "restore the original document" if original else f"run sync for {ref['source']}"
            html_links.append(f'{html.escape(label)}: unavailable; {html.escape(action)}')
            md_links.append(f"{_md(label)}: unavailable; {_md(action)}")
    return html_links, md_links


def _original_label(reference):
    identity = f"SHA-256 {reference['content_hash']}" if reference["content_hash"] else "content hash unavailable"
    return (f"{reference['path']} (repository-original article; author: {reference['attribution']}; "
            f"{identity}; untested educational material; no public license selected)")


def generate_index(root, output_dir="build"):
    root = Path(root).resolve()
    output = _output(root, output_dir)
    entries = catalog.catalog_entries(root)
    summary = catalog.counts(entries)
    html_parts = [
        '<p class="intro">An offline algorithm field guide. A source reference is not a local test result.</p>',
        '<a href="index.md">Read the Markdown index</a>',
        '<form id="filters" role="search"><label>Find an algorithm<input id="query" type="search" placeholder="Name, alias, tag or contract note" autocomplete="off"></label>',
    ]
    filters = {
        "category": sorted({e["category"] for e in entries}),
        "priority": ["core", "advanced"],
        "status": ["missing", "reference-only", "implemented", "tested", "failed", "stale"],
    }
    for key, choices in filters.items():
        html_parts.append(f'<label>{key.capitalize()}<select id="{key}"><option value="">All</option>')
        for value in choices:
            html_parts.append(f'<option value="{html.escape(value, quote=True)}">{html.escape(value)}</option>')
        html_parts.append("</select></label>")
    html_parts.extend([
        '</form><p id="result-count" role="status" aria-live="polite">'
        f'{len(entries)} entries</p><p id="no-results" hidden>No matches. Clear a filter or try another alias.</p>',
        '<noscript>All entries are shown. Browser search and the Markdown index work without JavaScript.</noscript>',
        '<div id="entries">',
    ])
    md_parts = ["# Offline algorithm index\n", "Source acquisition is not integration or local validation.\n",
                f"Individual entries: {len(entries)}.\n", "## Counts\n",
                *[f"- {key}: {value}" for key, value in summary["status"].items()], "\n## Algorithms\n"]
    search_data = []
    for entry in entries:
        identity = "entry-" + entry["id"]
        state = f"{entry['status']}; {entry['priority']}; {entry['availability']}"
        refs_html, refs_md = _references(entry, root, output / "index.html")
        html_parts.append(
            f'<article id="{identity}" class="entry" data-entry="{entry["id"]}">'
            f'<h2><a href="#{identity}">{html.escape(entry["name"])}</a></h2>'
            f'<p class="state">{html.escape(state)}</p>'
            f'<p class="taxonomy">{html.escape(entry["category"])} / {html.escape(entry["subcategory"])}</p>'
            f'<p>{html.escape(entry["contract"]["purpose"] or entry["reason"])}</p>'
            f'<p>{html.escape(entry["reason"])}</p>'
            f'<p class="aliases">Aliases: {html.escape(", ".join(entry["aliases"]) or "none")}</p>'
            f'<p>Tags: {html.escape(", ".join(entry["tags"]) or "none")}</p>'
            f'<p>Copy policy: {html.escape(entry["copy_policy"]["status"])}</p>'
            '<ul>' + "".join(f"<li>{ref}</li>" for ref in refs_html) + "</ul>"
        )
        md_parts += [f'<a id="{identity}"></a>\n', f"### {_md(entry['name'])}\n",
                     f"**{_md(state)}**\n", f"{_md(entry['category'])} / {_md(entry['subcategory'])}\n",
                     _md(entry["reason"]), f"Aliases: {_md(', '.join(entry['aliases']) or 'none')}\n",
                     f"Tags: {_md(', '.join(entry['tags']) or 'none')}\n",
                     f"Copy policy: {_md(entry['copy_policy']['status'])}\n",
                     *[f"- {link}" for link in refs_md]]
        if entry["integration"]:
            example = models.safe_path(root, entry["integration"]["example"])
            if example.is_file():
                link = _url(example, output / "index.html")
                html_parts.append(f'<p><a href="{link}">Complete standalone example</a></p>')
                md_parts.append(f"[Complete standalone example]({link})\n")
        html_parts.append("<details><summary>Contract and limitations</summary><dl>")
        for key, value in entry["contract"].items():
            html_parts.append(f"<dt>{html.escape(key.replace('_', ' '))}</dt><dd>{html.escape(value)}</dd>")
            md_parts.append(f"- **{_md(key.replace('_', ' '))}**: {_md(value)}")
        html_parts.append("</dl></details></article>")
        search_data.append({key: entry[key] for key in ("id", "name", "search_text", "category", "priority", "status")})
    html_parts.append("</div>")
    _write_outputs(root, output, {
        "index.html": _page("ICPC algorithm field guide", "".join(html_parts), search_data),
        "index.md": f"<!-- {GENERATED_MARKER} -->\n" + "\n".join(md_parts) + "\n",
        "style.css": _read(ASSETS / "style.css"), "search.js": _read(ASSETS / "search.js"),
    })
    return {"html": (output / "index.html").relative_to(root).as_posix(),
            "markdown": (output / "index.md").relative_to(root).as_posix(), "counts": summary}


def _closure(root, entry, sources):
    """Conservative static include closure, including extensionless ACL wrappers.

    Both preprocessor branches are inspected. Nonliteral includes fail instead
    of producing a notebook with silently unresolved prerequisites.
    """
    integration = entry["integration"]
    ids = list(integration["include_sources"]) if integration else [r["source"] for r in entry["references"]]
    for source_id in list(ids):
        ids.extend(dep for dep in sources[source_id]["managed_dependencies"] if dep not in ids)
    roots = [root / "include", root]
    for source_id in ids:
        base = catalog.source_root(root, sources[source_id])
        if base is None or not base.is_dir():
            raise ValueError(f"{entry['id']}: missing source {source_id}; run sync")
        roots.append(base)
    starts = []
    if integration:
        starts = [models.safe_path(root, integration["example"], True),
                  *[models.safe_path(root, helper, True) for helper in integration["helpers"]]]
    starts += [catalog.reference_path(root, sources[ref["source"]], ref) for ref in entry["references"] if ref["kind"] == "code"]
    discovered = OrderedDict()
    system_headers = set()

    def visit(path):
        if path is None:
            raise ValueError(f"{entry['id']}: source unavailable")
        path = path.resolve()
        if not path.is_relative_to(root):
            raise ValueError(f"{entry['id']}: dependency escapes toolkit root: {path}")
        if path in discovered:
            return
        text = _read(path)
        discovered[path] = text
        # Ignore commented-out directives while preserving line boundaries.
        directives = re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                            lambda match: ("\n" * match.group().count("\n")
                                           if match.group().startswith(("//", "/*")) else match.group()),
                            text, flags=re.DOTALL)
        if len(INCLUDE_DIRECTIVE.findall(directives)) != len(INCLUDE.findall(directives)):
            raise ValueError(f"{entry['id']}: nonliteral include in {path}; explicit dependency adaptation required")
        for delimiter, name in INCLUDE.findall(directives):
            if Path(name).is_absolute() or "\\" in name:
                raise ValueError(f"{entry['id']}: unsafe include {name}")
            candidates = ([path.parent] if delimiter == '"' else []) + roots
            dependency = next((base / name for base in candidates if (base / name).is_file()), None)
            if dependency is not None:
                visit(dependency)
            elif delimiter == '"' or name.startswith(("atcoder/", "toolkit/", "library/", "content/")):
                raise ValueError(f"{entry['id']}: unresolved dependency {name} included by {path}")
            else:
                system_headers.add(name)

    for start in starts:
        visit(start)
    return discovered, system_headers


def _file_permission(root, path, text, sources):
    relative = path.relative_to(root)
    if relative.parts[0] != "upstream":
        if relative.parts[0] not in {"include", "examples"}:
            raise ValueError(f"Notebook dependency outside allowed code roots: {relative}")
        return "Repository original/helper; no repository-wide license is selected."
    if len(relative.parts) < 4:
        raise ValueError(f"Invalid upstream dependency: {relative}")
    source = sources[relative.parts[1]]
    if relative.parts[2] != source["revision"]:
        raise ValueError(f"Dependency is not from the locked pin: {relative}")
    declared = source["license"]["declared"]
    selective = source["license"].get("copying") == "selective-per-file" or declared == "NOASSERTION"
    marker = re.search(r"License:\s*(CC0(?:-1\.0)?|Unlicense|MIT|BSD-[23]-Clause)\b", text[:5000], re.I)
    if marker:
        declared = marker.group(1)
        if declared.lower() in {"cc0", "cc0-1.0"}:
            declared = "CC0-1.0"
        elif declared.lower() == "unlicense":
            declared = "Unlicense"
    elif selective:
        raise ValueError(f"Unresolved file-level license permission: {relative}")
    if declared not in PERMITTED:
        raise ValueError(f"Unresolved license permission: {relative} ({declared})")
    if source["id"] == "ecnerwala" and "third_party" in relative.parts and not marker:
        raise ValueError(f"Third-party license needs an explicit reviewed file policy: {relative}")
    return f"{source['attribution'].get('author', source['name'])}; {declared}; pin {source['revision']}"


def generate_notebook(root, profile_path, output_dir="build/notebook"):
    root = Path(root).resolve()
    output = _output(root, output_dir)
    profile_path = Path(profile_path)
    if not profile_path.is_absolute():
        profile_path = root / profile_path
    if not profile_path.resolve().is_relative_to(root):
        raise ValueError("Notebook profile must be inside the toolkit root")
    document = catalog.load_catalog(root)
    profile = models.validate_profile(models.read_json(profile_path), document)
    entries = {entry["id"]: entry for entry in catalog.catalog_entries(root)}
    sources = models.source_map(models.load_sources(root))
    selected = [entries[entry_id] for entry_id in profile["entries"]]
    primary = set()
    dependencies = OrderedDict()
    notices = set()
    system_headers = set()
    prepared = []
    for entry in selected:
        study = entry["implementation"] == "reference-only" and profile.get("include_study_references", False)
        if not study and entry["status"] != "tested":
            raise ValueError(f"{entry['id']}: notebook requires current tested evidence; found {entry['status']}. Run test first.")
        if entry["availability"] != "available":
            raise ValueError(f"{entry['id']}: notebook source/example unavailable; run sync or restore declared files")
        if entry["copy_policy"]["status"] != "permitted":
            raise ValueError(f"{entry['id']}: copying blocked: {entry['reason']}")
        for ref in entry["references"]:
            if ref["license"] not in PERMITTED:
                raise ValueError(f"{entry['id']}: unresolved reference license {ref['license']}")
        closure, systems = _closure(root, entry, sources)
        system_headers.update(systems)
        snippets = []
        for ref in entry["references"]:
            path = catalog.reference_path(root, sources[ref["source"]], ref)
            text = _read(path)
            if ref["kind"] == "article":
                notices.add(_file_permission(root, path.resolve(), text, sources))
                notices.add(f"{sources[ref['source']]['attribution'].get('author', ref['source'])}; {ref['license']}; unchanged article source")
            else:
                notices.add(_file_permission(root, path.resolve(), text, sources))
            snippets.append((path.resolve(), text, ref["kind"]))
        if not entry["references"] and entry["integration"]:
            for helper in entry["integration"]["helpers"]:
                path = models.safe_path(root, helper, True).resolve()
                if path.name not in {"base.hpp", "kactl_prelude.hpp"}:
                    snippets.append((path, _read(path), "code"))
        originals = []
        if profile.get("include_study_references", False):
            for ref in entry["locations"]:
                if ref.get("provenance") == "repository-original":
                    path = models.local_reference_path(root, ref, check_paths=True)
                    label = _original_label(ref)
                    originals.append((path, _read(path), label, ref.get("fragment")))
                    notices.add(label)
        for path, text in closure.items():
            notices.add(_file_permission(root, path, text, sources))
            dependencies[path] = text
        primary.update(path for path, _, _ in snippets)
        example = None
        if entry["integration"]:
            path = models.safe_path(root, entry["integration"]["example"], True).resolve()
            example = path, _read(path)
            primary.add(path)
        notices.update(entry["copy_policy"]["notices"])
        prepared.append((entry, study, snippets, originals, example, list(closure)))
    body = [
        '<p class="intro">Independent snippets, not a single compilable translation unit. '
        'Retain contracts, dependencies and notices when adapting code.</p>',
        '<p>Core language target: GNU C++17 with GCC/libstdc++. The exact ETCPC compiler is not publicly verified.</p>',
        '<p><a href="notebook.md">Read the Markdown notebook</a>. Use browser Print to save a PDF; check your contest reference rules.</p>',
        "<nav aria-label=\"Contents\"><ol>",
    ]
    md = [f"# {_md(profile['name'])}\n", _md(profile.get("description", "")), "",
          "Independent snippets, **not a single compilable translation unit**. Preserve contracts, dependencies and notices.\n",
          "Target: GNU C++17 with GCC/libstdc++; the exact ETCPC compiler is not publicly verified.\n", "## Contents\n"]
    for entry in selected:
        body.append(f'<li><a href="#entry-{entry["id"]}">{html.escape(entry["name"])}</a></li>')
        md.append(f"- [{_md(entry['name'])}](#entry-{entry['id']})")
    body += ['</ol><a href="#dependencies">Dependency appendix</a> | <a href="#notices">Notices</a></nav>']
    dependency_ids = {path: f"dependency-{i}" for i, path in enumerate(sorted(set(dependencies) - primary))}

    def code_block(path, text, kind="code", fragment=None):
        label = path.relative_to(root).as_posix()
        link = _url(path, output / "notebook.html")
        if fragment:
            link += "#" + quote(fragment, safe="")
        body.append(f'<h4><a href="{link}">{html.escape(label)}</a></h4><pre><code>{html.escape(text)}</code></pre>')
        md.extend([f"#### [{_md(label)}]({link})\n", _fence(text, "cpp" if kind == "code" else "markdown")])

    for entry, study, snippets, originals, example, closure in prepared:
        body.append(f'<section id="entry-{entry["id"]}" class="notebook-entry"><h2>{html.escape(entry["name"])}</h2>')
        md += [f'\n<a id="entry-{entry["id"]}"></a>\n', f"## {_md(entry['name'])}\n"]
        label = "Study reference — untested educational/reference material, not a ready-to-use snippet." if study else "Current local baseline evidence: tested."
        body.append(f'<p class="state">{html.escape(label)}</p><dl>')
        md.append(label + "\n")
        for key, value in entry["contract"].items():
            body.append(f"<dt>{html.escape(key.replace('_', ' '))}</dt><dd>{html.escape(value)}</dd>")
            md.append(f"- **{_md(key.replace('_', ' '))}**: {_md(value)}")
        body.append("</dl>")
        links = [f'<a href="#{dependency_ids[path]}">{html.escape(path.relative_to(root).as_posix())}</a>'
                 for path in closure if path in dependency_ids]
        body.append("<p>Required appendix files: " + (", ".join(links) or "none") + "</p>")
        for path in closure:
            if path in dependency_ids:
                md.append(f"- Dependency: [{_md(path.relative_to(root).as_posix())}](#{dependency_ids[path]})")
        body.append("<h3>Snippet / reference content</h3>")
        md.append("\n### Snippet / reference content\n")
        for path, text, kind in snippets:
            code_block(path, text, kind)
        for path, text, provenance, fragment in originals:
            body.append(f'<p class="state">{html.escape(provenance)}</p>')
            md.append(_md(provenance) + "\n")
            code_block(path, text, "article", fragment)
        if example:
            body.append("<h3>Complete standalone example</h3>")
            md.append("\n### Complete standalone example\n")
            code_block(*example)
        body.append("</section>")
    body.append('<section id="dependencies"><h2>Dependency appendix</h2><p>Each required non-primary file appears once. '
                'System library headers are supplied by the compiler, not reproduced here.</p>')
    md += ['\n<a id="dependencies"></a>\n', "## Dependency appendix\n",
           "Required non-primary files are deduplicated. Keep the shown file layout or deliberately adapt includes; this is not an amalgamation.\n"]
    header_text = ", ".join(sorted(system_headers)) or "none"
    body.append(f"<p>Compiler/system headers: {html.escape(header_text)}</p>")
    md.append(f"Compiler/system headers: {_md(header_text)}\n")
    for path, identity in dependency_ids.items():
        body.append(f'<div id="{identity}">')
        md.append(f'<a id="{identity}"></a>\n')
        code_block(path, dependencies[path])
        body.append("</div>")
    body.append('</section><section id="notices"><h2>Notices</h2><ul>')
    md += ['\n<a id="notices"></a>\n', "## Notices\n"]
    for notice in sorted(notices):
        body.append(f"<li>{html.escape(notice)}</li>")
        md.append("- " + _md(notice))
    body.append("</ul>")
    used_sources = sorted({path.relative_to(root).parts[1] for path in dependencies if path.relative_to(root).parts[0] == "upstream"}
                          | {ref["source"] for entry in selected for ref in entry["references"]})
    for source_id in used_sources:
        source = sources[source_id]
        repository = source.get("repository_url")
        if repository:
            label = f"{source['name']} at {source['revision']} (external provenance)"
            link = repository + "/tree/" + source["revision"]
            body.append(f'<p><a href="{html.escape(link, quote=True)}">{html.escape(label)}</a></p>')
            md.append(f"[{_md(label)}]({link})\n")
        for evidence in source["license"].get("evidence", []):
            if evidence.upper().split("/")[-1] not in {"LICENSE", "COPYING", "LICENSE.TXT", "LICENSE.MD"}:
                continue
            path = models.safe_path(catalog.source_root(root, source), evidence, True)
            code_block(path, _read(path), "article")
    body.append("</section>")
    workflow = root / "docs/contest-workflow.md"
    if workflow.is_file():
        text = _read(workflow)
        body.append(f'<section id="contest-checklist"><h2>Contest workflow</h2><pre class="prose">{html.escape(text)}</pre></section>')
        md += ["\n## Contest workflow\n", text]
    _write_outputs(root, output, {"notebook.html": _page(profile["name"], "".join(body)),
                                 "notebook.md": f"<!-- {GENERATED_MARKER} -->\n" + "\n".join(md) + "\n",
                                 "style.css": _read(ASSETS / "style.css")})
    return {"html": (output / "notebook.html").relative_to(root).as_posix(),
            "markdown": (output / "notebook.md").relative_to(root).as_posix(),
            "entries": profile["entries"], "dependencies": len(dependency_ids)}


class _Links(HTMLParser):
    def __init__(self):
        super().__init__()
        self.links = []
        self.anchors = set()

    def handle_starttag(self, tag, attrs):
        values = dict(attrs)
        if "id" in values:
            self.anchors.add(values["id"])
        if tag == "a" and "name" in values:
            self.anchors.add(values["name"])
        for key in ("href", "src"):
            if key in values:
                self.links.append(values[key])


def _document_links(path):
    text = _read(path)
    if path.suffix == ".md":
        text = models.markdown_prose(text)
    parser = _Links()
    parser.feed(text)
    if path.suffix == ".md":
        parser.links.extend(re.findall(r"(?<!!)\[[^\n]*?\]\(([^)\s]+)\)", text))
        parser.anchors.update(models.markdown_heading_anchors(text))
    return parser


def validate_links(root, output_dir=None):
    root = Path(root).resolve()
    output = _output(root, output_dir or "build")
    if not output.exists():
        raise ValueError(f"Generated output missing: {output}; run index/notebook first")
    errors = []
    cache = {}
    documents = []
    # A build directory may contain a clean-checkout exercise with its own raw
    # upstream websites. Validate toolkit output, not every acquired website.
    excluded = {"upstream", ".git", "tests", "specs", ".specify", ".github"}
    for directory, dirs, files in os.walk(output, followlinks=False):
        dirs[:] = sorted(name for name in dirs if name not in excluded)
        for name in sorted(files):
            if name not in {"index.html", "index.md", "notebook.html", "notebook.md"}:
                continue
            path = Path(directory) / name
            if GENERATED_MARKER in _read(path)[:500]:
                documents.append(path)
    if not documents:
        raise ValueError(f"No generated toolkit documents in {output}; run index/notebook first")
    for document in documents:
        if not document.resolve().is_relative_to(root):
            errors.append(f"{document}: document escapes toolkit root")
            continue
        parsed = _document_links(document)
        cache[document.resolve()] = parsed
        for link in parsed.links:
            parts = urlsplit(link)
            if parts.scheme in {"http", "https", "mailto"}:
                continue
            if parts.scheme or parts.netloc or parts.query:
                errors.append(f"{document}: unsupported local link {link}")
                continue
            lexical = Path(os.path.normpath(document.parent / unquote(parts.path))) if parts.path else document
            if lexical.is_relative_to(root) and lexical.relative_to(root).parts[:2] == ("docs", "techniques"):
                ref = {"path": lexical.relative_to(root).as_posix()}
                if parts.fragment:
                    ref["fragment"] = unquote(parts.fragment)
                try:
                    models.local_reference_path(root, ref, check_paths=True)
                except ValueError as exc:
                    errors.append(f"{document}: {exc}")
                    continue
            target = lexical.resolve()
            if not target.is_relative_to(root):
                errors.append(f"{document}: link escapes toolkit root: {link}")
            elif not target.is_file():
                errors.append(f"{document}: broken local link: {link}")
            elif parts.fragment:
                if target not in cache:
                    cache[target] = _document_links(target)
                if unquote(parts.fragment) not in cache[target].anchors:
                    errors.append(f"{document}: missing anchor: {link}")
    return errors
