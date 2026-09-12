#pragma once
#include "base.hpp"
#include "library/string/suffix_automaton.hpp"

namespace toolkit {
// Independently authored immutable ownership adapter over unchanged Suisen CC0.
// Owns the automaton behind its occurrence counter: copies survive the source
// object's destruction. No append API or borrowed mutable state is exposed.
// All bytes are accepted; distinct_substrings excludes empty, occurrences counts
// overlaps and gives n+1 for empty. Queries are O(|pattern| log(256)).
class SuffixAutomaton {
    using Engine = suisen::SuffixAutomaton<char>;
    shared_ptr<const Engine> engine_;
    Engine::SubstringCounter counter_;
    long long distinct_ = 0;

    static shared_ptr<const Engine> build(const string& text) {
        if (text.size() > size_t((INT_MAX - 1) / 2))
            throw length_error("suffix automaton state index bound");
        return make_shared<const Engine>(text);
    }
public:
    explicit SuffixAutomaton(const string& text)
        : engine_(build(text)), counter_(engine_->substring_counter()) {
        for (size_t v = 1; v < engine_->nodes.size(); ++v) {
            const auto& node = engine_->nodes[v];
            distinct_ += node.len - engine_->nodes[node.link].len;
        }
    }
    bool contains(const string& pattern) const { return engine_->accept(pattern); }
    long long occurrences(const string& pattern) const { return counter_.count(pattern); }
    long long distinct_substrings() const { return distinct_; }
};
}
