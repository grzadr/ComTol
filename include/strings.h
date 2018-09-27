#pragma once

#include "basic.h"

namespace AGizmo {

namespace StrTol {

// Function checks if given string contains any characters and only digits.
// It rejects numbers with decimal point and written in scientific notation.
inline bool only_digits(const string &query) noexcept {
    return (query.size() &&
            find_if_not(begin(query), end(query),
                        [](const char &c) { return isdigit(c); }) == end(query));
}

// Function checks if given strings is a valid number and converts it to
// integer. If it is not a valid number function returns nullopt.
inline opt_int strtoi(const string &query) {
  if (only_digits(query))
    return std::stoi(query);
  return nullopt;
}

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

// Function counts all occurences of character in string.
// Check starts from pos position. If pos is string::npos
// it checks from the beginning of the string.
// If pos is bigger than string size it return 0.
inline auto count_all(const string &source, char query, size_t pos = 0) {
    auto first =
            next(begin(source), (pos == string::npos) ? 0 : static_cast<long>(pos));
    auto last = end(source);
    if (first > last)
        return 0l;
    return count(first, last, query);
}

// Function cleans string's ends from whitespace characters.
// It find first and last characters that are not whitespace and construct
// string from iterators

// Function cleans string's ends from whitespace characters.
// It find first and last characters that are not whitespace and
// substring from this positions.
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
                        const string &strip = "") {
    // If user defines collection of whitespaces overloaded function is called
    if (strip.size())
        return str_clean(source, strip.c_str(), ends);

    // If ends is true ends of source are trimmed.
    string result{ends ? str_clean_ends(source) : source};
    auto first = result.begin();
    auto last = result.end();
    replace_if(first, last, [](char c) { return isspace(c); }, ' ');
    last = unique(first, result.end(),
                  [](char l, char r) { return (l == ' ' && r == ' '); });
    return string(first, last);
}

inline vec_str str_split(const string &source, char sep, bool empty) {
    if (!source.size())
        return vec_str{};

    vec_str result{};

    split<string>(source.begin(), source.end(), sep, back_inserter(result));

    if (!empty)
        result.erase(remove(begin(result), end(result), ""), end(result));

    return result;
}

inline vec_str str_split(const string &source, string sep = "\t",
                         bool empty = true) {
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

inline vec_str str_split(const string &source, size_t count) {
    if (!count || source.length() <= count)
        return {source};

    vec_str result = {};
    size_t last = 0;

    do {
        result.push_back(source.substr(last, count));
        last += count;
    } while (last < source.length() - count);

    result.push_back(source.substr(last, count));

    return result;
}

inline string str_align(const string &message, size_t width = 80,
                        char align = 'c') {
    if (!message.size() || message.size() >= width)
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

    if (prefix.size() + suffix.size() >= width)
        throw runtime_error("Message width is smaller than frame!");

    if (prefix.size())
        width -= prefix.size() + 1;
    if (suffix.size())
        width -= suffix.size() + 1;

    if (message_size <= width) {
        return (prefix.size() ? prefix + " " : "") +
               str_align(message, width, align) +
               (suffix.size() ? " " + suffix : "");
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
                prev = message.find_last_of(" ", last);

            result += (prefix.size() ? prefix + " " : "") +
                      str_align(message.substr(first, prev - first), width, align) +
                      (suffix.size() ? " " + suffix : "") + "\n";

            first = prev + 1;
            last = first + width;

        } while (prev < message_size);
        result.pop_back();
        return result;
    }
}

template <typename It> string str_join(It begin, It end, string sep = "\t") {
    if (begin == end)
        return "";
    return accumulate(next(begin), end, *begin,
                      [sep](string a, string b) { return a + sep + b; });
}

template <typename Container>
string str_join(const Container &container, string sep = "\t") {
    return str_join(container.begin(), container.end(), sep);
}

template <typename Value = opt_str, typename It>
string join_fields(It first, It last, string fields, string values) {

    vector<string> temp = {};
    temp.reserve(static_cast<size_t>(distance(first, last)));

    if constexpr (is_same_v<opt_str, Value>)
        transform(first, last, back_inserter(temp), [&values](const auto &ele) {
            if (auto value = ele.second)
                return ele.first + values + *value;
            else
                return ele.first;
        });
    else
        transform(first, last, back_inserter(temp), [&values](const auto &ele) {
            return ele.first + values + ele.second;
        });

    return str_join(temp, fields);
}

template <typename Value = opt_str, typename Map, typename It>
string join_fields(const Map &ref, It first, It last, string fields,
                   string values) {

    vector<string> temp = {};
    temp.reserve(static_cast<size_t>(distance(first, last)));

    if constexpr (is_same_v<opt_str, Value>)
        transform(first, last, back_inserter(temp),
                  [&ref, &values](const auto &ele) {
                      if (auto value = ref.at(ele))
                          return ele + values + *value;
                      else
                          return ele;
                  });
    else
        transform(first, last, back_inserter(temp),
                  [&ref, &values](const auto &ele) {
                      return ele + values + ref.at(ele);
                  });

    return str_join(temp, fields);
}

inline string str_join_fields(const umap_str_opt &ref, string fields = ";",
                              string values = "=") {
    return join_fields(ref.begin(), ref.end(), fields, values);
}

inline string str_join_fields(const umap_str &ref, string fields = ";",
                              string values = "=") {
    return join_fields<string>(ref.begin(), ref.end(), fields, values);
}

inline string str_join_fields(const umap_str &ref, const vec_str &names,
                              string fields = ";", string values = "=") {
    return join_fields<string>(ref, names.begin(), names.end(), fields, values);
}

inline string str_join_fields(const umap_str_opt &ref, const vec_str &names,
                              string fields = ";", string values = "=") {
    return join_fields(ref, names.begin(), names.end(), fields, values);
}

inline string str_devide(const string &source, size_t count, string sep) {
    if (!count || !source.length() || source.length() <= count)
        return source;
    else
        return str_join(str_split(source, count), sep);
}

inline string str_reverse(string result) {
    if (!result.length())
        return "";
    std::reverse(result.begin(), result.end());
    return result;
}

template <typename Ite>
vec_str str_chunks(const string &source, Ite begin, Ite end) {
    vec_str result = {};
    size_t last = 0;

    while (begin != end) {
        if (last >= source.length())
            break;
        else if (*begin < 0)
            ++begin;
        else {
            result.push_back(source.substr(last, *begin));
            last += *(begin++);
        }
    }

    if (last < source.length())
        result.push_back(source.substr(last));

    return result;
}

inline umap_str vec_str_map(const vec_str &keys, const vec_str &values) {
  if (!keys.size() || keys.size() != values.size())
    throw runtime_error(
            "Invalid lengths of vectors: " + to_string(keys.size()) + " and " +
            to_string(values.size()));
  umap_str result{};

  mapify(keys.begin(), keys.end(), values.begin(), values.end(),
         inserter(result, result.begin()));

  if (result.size() != keys.size())
    throw runtime_error("Duplicated keys in vector with keys");

  return result;

inline umap_str str_map(const string &source, const vec_str &header,
                        bool clean = true, const string &sep = "\t") {
    if (!source.size())
        throw runtime_error("Empty Source!");

    auto splitted = StrTol::str_split(source, sep, true);

    if (splitted.size() != header.size())
        throw runtime_error("Source (" + to_string(splitted.size()) +
                            ") and Header(" + to_string(header.size()) +
                            ") differs in size!\n");

    if (clean)
        transform(splitted.begin(), splitted.end(), splitted.begin(),
                  [](const string &ele) { return str_clean(ele); });

    return vec_str_map(header, splitted);
}

// Converts string containing fields separated with char fields and
// their values seperated by char values into map. If some field has no value,
// nullopt is inserted.
// Example: "K1=V1;K2;K3=" -> {{"K1", "V1"}, {"K2", nullopt}, "
inline umap_str_opt str_map_fields(const string &source, char fields = ';',
                                   char values = '=') {
    if (!source.size())
        return {};

    umap_str_opt result{};

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

// Replaces all occurences of string query with string value in string source.
// inline void str_replace(string& source, char query, char value = 0){
inline void str_replace(string &source, char query) {
    source.erase(remove(source.begin(), source.end(), query), source.end());
}

inline void str_replace(string &source, char query, char value) {
    replace(source.begin(), source.end(), query, value);
}

// Replaces all occurences of string query with string value in string source.
inline void str_replace(string &source, const string &query,
                        const string &value) {

    if (auto query_size = static_cast<long>(query.size());
            !source.size() || !query_size)
        return;
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
    } else if (value_size)
        str_replace(source, query[0], value[0]);
    else
        str_replace(source, query[0]);
}

template <typename T>
inline void str_replace_n(string &source, const T &index) {
    for (const auto &[ele, value] : index)
        str_replace(source, ele, value);
}

template <typename T>
inline string str_replace_n_copy(string source, const T &index) {
    str_replace_n(source, index);
    return source;
}

inline void open_file(const string &file_name, ifstream &stream) {

    //    auto file_path = path(file_name);
    //    if (exists(file_path)) {
    stream = ifstream(file_name);
    if (!stream.is_open())
        throw runerror{"Can't open '" + file_name + "'\n"};
    //    } else
    //      throw runerror{"File '" + file_name + "' does not exist"};
}

} // namespace StrTol

}