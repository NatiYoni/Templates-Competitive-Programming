#pragma once
#include <bits/stdc++.h>
using namespace std;
#ifndef TOOLKIT_SEED
#define TOOLKIT_SEED 1729
#endif
inline mt19937 rng(TOOLKIT_SEED);
inline int cases_checked = 0;
template<class T> string show(const vector<T>& values) {
    ostringstream out;
    out << '[';
    for (size_t i = 0; i < values.size(); ++i) out << (i ? "," : "") << values[i];
    return out.str() + ']';
}
inline void require(bool condition, const string& input) {
    if (!condition) {
        cerr << "FAIL seed=" << TOOLKIT_SEED << " case=" << cases_checked
             << " input=" << input << '\n';
        exit(1);
    }
}
inline void success() {
    cout << "OK cases=" << cases_checked << " seed=" << TOOLKIT_SEED << '\n';
}
