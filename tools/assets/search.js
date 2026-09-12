"use strict";
(() => {
  const node = document.getElementById("search-data");
  if (!node) return;
  const entries = JSON.parse(node.textContent);
  const query = document.getElementById("query");
  const filters = ["category", "priority", "status"];
  // Metadata is already casefolded by Python; normalize common expanding folds
  // as well, so Unicode names such as Straße remain searchable offline.
  const fold = text => text.toLowerCase().replace(/ß/g, "ss").replace(/ς/g, "σ");
  function update() {
    const text = fold(query.value);
    let count = 0;
    for (const entry of entries) {
      const visible = entry.search_text.includes(text) && filters.every(key => {
        const value = document.getElementById(key).value;
        return !value || entry[key] === value;
      });
      document.getElementById("entry-" + entry.id).hidden = !visible;
      if (visible) count++;
    }
    document.getElementById("result-count").textContent = `${count} of ${entries.length} entries`;
    document.getElementById("no-results").hidden = count !== 0;
  }
  document.getElementById("filters").addEventListener("submit", event => event.preventDefault());
  query.addEventListener("input", update);
  for (const key of filters) document.getElementById(key).addEventListener("change", update);
  update();
})();
