#pragma once

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using std::is_same_v;
using std::map;
using std::nullopt;
using std::optional;
using std::pair;
using std::setw;
using std::size_t;
using std::string;
using std::string_view;
using std::unordered_map;
using std::vector;
using sstream = std::stringstream;
using std::ifstream;

using std::cout;
using std::endl;

using std::back_inserter;
using std::begin;
using std::end;
using std::inserter;

using std::accumulate;
using std::boyer_moore_horspool_searcher;
using std::boyer_moore_searcher;
using std::count;
using std::default_searcher;
using std::find_if_not;
using std::isspace;
using std::runtime_error;
using std::search;
using std::to_string;
using std::unique;

template <typename It> using bmhs = boyer_moore_horspool_searcher<It>;
template <typename It> using defs = default_searcher<It>;
using vec_str = vector<string>;
using opt_int = optional<int>;
using opt_str = optional<string>;
using pair_str = pair<string, string>;

using map_str = map<string, string>;
using map_str_opt = map<string, opt_str>;

template <typename Key, typename Value> using umap = unordered_map<Key, Value>;
using umap_str = umap<string, string>;
using umap_str_opt = umap<string, opt_str>;

using chours = std::chrono::hours;
using cminutes = std::chrono::minutes;
using cseconds = std::chrono::seconds;

using runerror = std::runtime_error;
