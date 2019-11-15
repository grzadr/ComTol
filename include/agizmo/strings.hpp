#pragma once

#include <iostream>

#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>

#include "basic.hpp"

//#include <experimental/iterator>

namespace AGizmo {

using std::string;
using sstream = std::stringstream;

using std::nullopt;
using opt_int = std::optional<int>;
using opt_str = std::optional<string>;

using vec_str = vector<string>;

using chours = std::chrono::hours;
using cminutes = std::chrono::minutes;
using cseconds = std::chrono::seconds;

namespace StringSearch {

// Function counts all occurences of character in string.
// Check starts from pos position. If pos is string::npos
// it checks from the beginning of the string.
// If pos is bigger than string size it returns 0.
inline auto contains(const string &source, const char query, size_t pos = 0) {
  return source.find(query, pos) != string::npos;
}

inline long count_all(const string &source, char query, size_t pos = 0) {
  auto first = next(begin(source), (pos == string::npos) ? 0 : pos);
  auto last = end(source);

  if (first <= last)
    return std::count(first, last, query);
  else
    return 0;
}

inline auto str_starts_with(const string &source, const string &query) {
  return query.size() <= source.size() &&
         query == source.substr(0, query.size());
}

inline auto str_starts_with(const string &source, const char query) {
  return 1 <= source.size() && query == source.front();
}

inline auto str_ends_with(const string &source, const string &query) {
  return query.size() <= source.size() &&
         query == source.substr(source.size() - query.size());
}

inline auto str_ends_with(const string &source, const char query) {
  return 1 <= source.size() && query == source.back();
}

} // namespace StringSearch

namespace StringFormat {

// Function checks if given string contains any characters and only digits.
// It rejects numbers with decimal point and written in scientific notation.
inline bool only_digits(const string &query) noexcept {
  return (!query.empty() &&
          find_if_not(begin(query), end(query),
                      [](const char &c) { return isdigit(c); }) == end(query));
}

// Function checks if given strings is a valid number and converts it to
// integer. If it is not a valid number function returns nullopt.
inline opt_int str_to_int(const string &query, bool negative = false) {
  if (negative && query.front() == '-' && only_digits(query.substr(1)))
    return std::stoi(query);
  else if (only_digits(query))
    return std::stoi(query);
  else
    return nullopt;
}

// Function converts time to string
template <typename Type> string str_time(std::chrono::duration<Type> time) {
  auto hours = to_string(std::chrono::floor<chours>(time).count());
  auto minutes = to_string(std::chrono::floor<cminutes>(time).count() % 60);
  auto seconds = to_string(std::chrono::floor<cseconds>(time).count() % 60);

  sstream message;

  message << std::setfill('0') << std::right << std::setw(2) << hours << ":"
          << std::right << std::setw(2) << minutes << ":" << std::right
          << std::setw(2) << seconds;

  return message.str();
}

using std::abs;
using std::log10;

inline string str_double(double number, int step = 3, int precision = 0) {
  sstream output;

  if (step) {
    if (const auto magnitude = log10(number); abs(magnitude) > step) {
      output << std::scientific << std::setprecision(precision ? precision : 3)
             << number;
    } else
      output << std::setprecision(step) << number;
  } else {
    if (precision)
      output << std::fixed << std::setprecision(precision) << number;
    else
      output << std::setprecision(std::numeric_limits<double>::digits10 + 1)
             << number;
  }

  return output.str();
}

// Function cleans string's ends from whitespace characters.
// It find first and last characters that are not whitespace and construct
// string from iterators
inline string str_clean_ends(const string &source, const char strip[]) {
  // Find first character not present in strip array.
  auto first = source.find_first_not_of(strip);
  // If there are only whitespaces or source is empty return an empty string
  if (string::npos == first)
    return {};
  // Find last character that is not in strip.
  auto last = source.find_last_not_of(strip);
  // Return subtring from first to last position
  return source.substr(first, last - first + 1);
}

// Function cleans string's ends from whitespace characters.
// It find first and last characters that are not whitespace and
// substring from this positions.
inline string str_clean_ends(const string &source, const string &strip = "") {
  if (strip.size())
    return str_clean_ends(source, strip.c_str());

  auto first = find_if_not(source.begin(), source.end(),
                           [](char c) { return isspace(c); });
  if (source.end() == first)
    return {};
  auto last = find_if_not(source.rbegin(), source.rend(),
                          [](char c) { return isspace(c); });
  return string(first, last.base());
}

inline string str_clean(const string &source, const char strip[],
                        bool ends = true) {
  string result{ends ? str_clean_ends(source, strip) : source};

  auto found = result.find_first_of(strip);
  while (string::npos != found) {
    result.at(found) = ' ';
    found = result.find_first_of(strip, found + 1);
  }
  auto first = result.begin();
  auto last = unique(first, result.end(),
                     [](char l, char r) { return (l == ' ' && r == ' '); });

  return string(first, last);
}

// Function cleans string from multiple whitespace characters.by converting
// all whitespaces into regular spaces and reducing consecutive spaces
// into single ones.
inline string str_clean(const string &source, bool ends = true,
                        const string &strip = " \n") {
  // If user defines collection of whitespaces overloaded function is called
  if (!strip.empty())
    return str_clean(source, strip.c_str(), ends);

  string result{ends ? str_clean_ends(source) : source};
  auto first = result.begin();
  auto last = result.end();
  replace_if(
      first, last, [](char c) { return isspace(c); }, ' ');
  last = unique(first, result.end(),
                [](char l, char r) { return (l == ' ' && r == ' '); });
  return string(first, last);
}

inline string str_align(const string &message, size_t width = 80,
                        char align = 'c') {
  if (message.empty() || message.size() >= width)
    return message;

  string result(width, ' ');

  switch (align) {
  case 'c':
    result.replace((width - message.size()) / 2, message.size(), message);
    break;
  case 'r':
    result.replace(width - message.size(), message.size(), message);
    break;
  default:
    result.replace(0, message.size(), message);
    break;
  }

  return result;
}

inline string str_frame(string message, size_t width, const string &prefix,
                        const string &suffix, char align = 'c') {
  message = str_clean(message);

  const size_t message_size = message.size();

  if (!message_size)
    return {};

  const size_t prefix_size = prefix.size();
  const size_t suffix_size = suffix.size();

  if (prefix_size + suffix_size >= width)
    throw runtime_error("Message width is smaller than frame!");

  if (prefix_size)
    width -= prefix_size + 1;
  if (suffix_size)
    width -= suffix_size + 1;

  if (message_size <= width) {
    return (prefix_size ? prefix + " " : "") +
           str_align(message, width, align) + (suffix_size ? " " + suffix : "");
  } else {
    string result{};

    size_t first{0};
    size_t prev{0};
    size_t last{width};

    do {
      prev = message.find_first_of(' ', first);
      if (last >= message_size or prev > message_size)
        prev = message_size;
      else if (last > prev)
        prev = message.find_last_of(' ', last);

      result += (prefix_size ? prefix + " " : "") +
                str_align(message.substr(first, prev - first), width, align) +
                (suffix_size ? " " + suffix : "") + "\n";

      first = prev + 1;
      last = first + width;

    } while (prev < message_size);
    result.pop_back();
    return result;
  }
}

// Replaces all occurences of string query with string value in string source.
// inline void str_replace(string& source, char query, char value = 0){
inline string str_replace(string source, char query) {
  source.erase(remove(source.begin(), source.end(), query), source.end());
  return source;
}

inline string str_replace(string source, char query, char value) {
  replace(source.begin(), source.end(), query, value);
  return source;
}

// Replaces all occurences of string query with string value in string source.
inline string str_replace(string source, const string &query,
                          const string &value) {
  if (auto query_size = static_cast<long>(query.size());
      !source.size() || !query_size)
    return source;
  else if (auto value_size = value.size(); query.size() > 1 || value_size > 1) {
    auto first = source.begin();
    auto searcher = default_searcher(query.begin(), query.end());
    auto found = search(first, source.end(), searcher);
    auto visited = distance(first, found);

    while (found != source.end()) {
      visited += value_size;
      source.replace(found, found + query_size, value);
      first = next(source.begin(), visited);
      found = search(first, source.end(), searcher);
      visited += distance(first, found);
    }
    return source;
  } else if (value_size)
    return str_replace(source, query[0], value[0]);
  else
    return str_replace(source, query[0]);
}

template <typename T>
inline string str_replace_n(string source, const T &index) {
  for (const auto &[ele, value] : index)
    str_replace(source, ele, value);
  return source;
}

inline string str_reverse(string result) {
  if (!result.length())
    return "";
  reverse(result.begin(), result.end());
  return result;
}

inline string str_remove_prefix(const string &source, const string &prefix) {
  if (!StringSearch::str_starts_with(source, prefix))
    return source;
  else
    return source.substr(prefix.length());
}

inline string str_remove_prefix(const string &source, const char prefix) {
  if (!StringSearch::str_starts_with(source, prefix))
    return source;
  else
    return source.substr(1);
}

inline string str_remove_suffix(const string &source, const string &suffix) {
  if (!StringSearch::str_ends_with(source, suffix))
    return source;
  else
    return source.substr(0, source.length() - suffix.length());
}

inline string str_remove_suffix(const string &source, const char suffix) {
  if (!StringSearch::str_ends_with(source, suffix))
    return source;
  else
    return source.substr(0, source.length() - 1);
}

} // namespace StringFormat

namespace StringCompose {

//#include <experimental/iterator>
// using std::experimental::ostream_joiner;

template <typename It> string str_join(It begin, It end, string sep = "\t") {
  if (begin == end)
    return "";

  sstream output;
  output << *begin;
  for_each(next(begin), end,
           [&output, sep](const auto &ele) { output << sep << ele; });
  //  copy(begin, end, ostream_joiner(output, sep));

  return output.str();
}

template <typename It> string str_join(It begin, It end, char sep) {
  return str_join(begin, end, string(1, sep));
}

template <typename Container>
string str_join(const Container &container, string sep = "\t") {
  return str_join(container.begin(), container.end(), sep);
}

template <typename Container>
string str_join(const Container &container, char sep) {
  return str_join(container.begin(), container.end(), string(1, sep));
}

template <typename It>
string str_join_quoted(It begin, It end, string sep = "\t") {
  if (begin == end)
    return "";

  sstream output;
  output << quoted(*begin);
  for_each(next(begin), end,
           [&output, sep](const auto &ele) { output << sep << quoted(ele); });

  return output.str();
}

template <typename It> string str_join_quoted(It begin, It end, char sep) {
  return str_join_quoted(begin, end, string(1, sep));
}

template <typename Container>
string str_join_quoted(const Container &container, string sep = "\t") {
  return str_join_quoted(container.begin(), container.end(), sep);
}

template <typename Container>
string str_join_quoted(const Container &container, char sep) {
  return str_join_quoted(container.begin(), container.end(), string(1, sep));
}

template <class InputIt>
inline string str_join_fields(InputIt first, const InputIt &last,
                              string fields_sep = ";",
                              string values_sep = "=") {
  if (first == last)
    return "";

  sstream output;

  for (; first != last; ++first) {
    const auto &[key, value] = *first;
    output << fields_sep << key;
    if (value)
      output << values_sep << *value;
  }

  string result;

  output.get();
  output >> result;

  return result;
}

template <class MapType>
inline string str_join_fields(const MapType &values, string fields_sep = ";",
                              string values_sep = "=") {
  return str_join_fields(values.begin(), values.end(), fields_sep, values_sep);
}

template <class Missing, class InputIt>
inline string str_join_fields(Missing missing, InputIt first,
                              const InputIt &last, string fields_sep = ";",
                              string values_sep = "=") {
  if (first == last)
    return "";

  sstream output;

  for (; first != last; ++first) {
    const auto &[key, value] = *first;
    output << fields_sep << key;
    if (value != missing)
      output << values_sep << value;
  }

  string result;

  output.get();

  output >> result;

  return result;
}

// template <typename Value = opt_str, typename Map, typename It>
// inline string str_join_fields(const Map &ref, It first, It last,
//                              string fields_sep = ";",
//                              string values_sep = "=") {

//  vector<string> temp = {};
//  temp.reserve(static_cast<size_t>(distance(first, last)));

//  if constexpr (is_same_v<opt_str, Value>)
//    transform(first, last, back_inserter(temp),
//              [&ref, &values_sep](const auto &ele) {
//                if (auto value = ref.at(ele))
//                  return ele + values_sep + *value;
//                else
//                  return ele;
//              });
//  else
//    transform(first, last, back_inserter(temp),
//              [&ref, &values_sep](const auto &ele) {
//                return ele + values_sep + ref.at(ele);
//              });

//  return str_join(temp, fields_sep);
//}

} // namespace StringCompose

namespace StringDecompose {

using Basic::segment;
using Basic::split;

inline vec_str str_split(const string &source, char sep, bool empty) {
  if (!source.size())
    return vec_str{};

  vec_str result{};

  split<string>(source.begin(), source.end(), sep, back_inserter(result));

  if (!empty)
    result.erase(remove(begin(result), end(result), ""), end(result));

  return result;
}

inline vec_str str_split_quoted(const string &source, char sep, char quote) {
  if (!source.size())
    return vec_str{};

  vec_str output{};
  string temp{};
  for (const auto &ele : str_split(source, sep, true)) {
    // Debug
    //    std::cerr << "temp.empty(): " << temp.empty() << "\n"
    //              << "count: " << StringSearch::count_all(ele, quote) << "\n"
    //              << ele << "\n";
    if (ele.empty()) {
      if (temp.empty())
        output.emplace_back(ele);
      else
        temp + sep + sep;
    } else {
      if (temp.empty()) {
        if (StringSearch::count_all(ele, quote) % 2 == 0)
          output.emplace_back(ele);
        else
          temp = ele;
      } else {
        if (auto count = StringSearch::count_all(ele, quote); !count)
          temp += sep + ele;
        else if (count % 2 == 0)
          throw runtime_error{
              source + "\n\n includes unclosed quatiotation in \n\n" + ele};
        else {
          output.emplace_back(temp + ele);
          temp = "";
        }
      }
    }
  }

  if (!temp.empty())
    throw runtime_error{"'" + temp + "' is not empty '" + "' "};

  return output;
}

inline vec_str str_split(const string &source, string sep = "\t",
                         bool empty = true) {
  if (sep.empty() || source.empty())
    return vec_str{source};

  if (sep.size() == 1)
    return str_split(source, sep[0], empty);

  vec_str result{};

  if (sep.length() > 5)
    split<string, bmhs>(begin(source), end(source), begin(sep), end(sep),
                        back_inserter(result));
  else
    split<string, defs>(begin(source), end(source), begin(sep), end(sep),
                        back_inserter(result));

  if (!empty)
    result.erase(remove(begin(result), end(result), ""), end(result));

  return result;
}

inline vec_str str_segment(const string &source, size_t length) {
  vec_str result = {};

  segment<string>(source.begin(), source.end(), back_inserter(result), length);

  return result;
}

inline string str_segment(const string &source, size_t length, string sep) {
  if (!length || source.empty() ||
      //      source.length() <= static_cast<size_t>(length))
      source.length() <= length)
    return source;
  else
    return StringCompose::str_join(str_segment(source, length), sep);
}

using std::optional;
using opt_str = optional<string>;
using map_str_opt = std::unordered_map<string, opt_str>;

inline map_str_opt str_map_fields(const string &source, char fields = ';',
                                  char values = '=') {
  if (!source.size())
    return {};

  map_str_opt result{};

  for (auto ele : str_split(source, fields, true)) {
    const auto mark(ele.find(values));

    string key{ele.substr(0, mark)};
    auto value{string::npos == mark ? opt_str{} : ele.substr(mark + 1)};

    if (auto [it, inserted] = result.try_emplace(key, value); !inserted) {
      if (auto value = (*it).second)
        throw runtime_error("Key " + key + "already in map -> " + *value);
      else
        throw runtime_error("Key " + key + "already in map -> None");
    }
  }

  return result;
}

using str_pair = std::pair<string, string>;

inline str_pair str_split_in_half(const string &source, char mark) noexcept {
  if (source.empty())
    return {"", ""};

  if (auto pos = source.find(mark);
      pos == string::npos || pos == source.size() - 1)
    return {source, ""};
  else if (pos == 0)
    return {"", source};
  else
    return {source.substr(0, pos), source.substr(pos + 1)};
}

inline string str_extract_before(const string &source,
                                 const char &terminator) noexcept {
  if (source.empty())
    return "";

  if (auto pos = source.find_first_of(terminator); pos != string::npos)
    return source.substr(0, pos);
  else
    return source;
}

} // namespace StringDecompose

} // namespace AGizmo
