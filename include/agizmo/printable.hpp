#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <agizmo/strings.hpp>

namespace AGizmo::Printable {
using std::cout;
using std::endl;
using std::fixed;
using std::get;
using std::holds_alternative;
using std::is_same_v;
using std::optional;
using std::ostream;
using std::setprecision;
using std::string;
using std::variant;
using std::vector;

template <typename Type>
struct PrintableVector {
  vector<Type> value{};

  PrintableVector() = default;
  PrintableVector(std::initializer_list<Type> input) : value{input} {}
  template <class InputIt>
  PrintableVector(InputIt first, InputIt last) {
    value = vector<Type>(first, last);
  }
  PrintableVector(vector<Type> input) : value{input} {}

  string str() const {
    if (value.empty()) return "{}";

    return "{" + StringCompose::str_join(value.begin(), value.end(), ",") + "}";
  }

  auto begin() noexcept { return value.begin(); }
  auto cbegin() const noexcept { return value.cbegin(); }
  auto end() noexcept { return value.end(); }
  auto cend() const noexcept { return value.cend(); }

  bool operator==(const PrintableVector<Type> &other) const {
    return this->value == other.value;
  }

  bool operator!=(const PrintableVector<Type> &other) const {
    return !(*this == other);
  }

  friend ostream &operator<<(ostream &stream, const PrintableVector &item) {
    return stream << item.str();
  }
};

template <typename Type = int>
struct NestedVector {
  vector<PrintableVector<Type>> values{};

  NestedVector() = default;
  NestedVector(std::initializer_list<PrintableVector<Type>> input)
      : values{input} {}

  string str() const {
    if (values.empty())
      return "{{}}";
    else {
      sstream output;
      for (const auto &ele : values) {
        output << ", {" + StringCompose::str_join(ele.begin(), ele.end(), ",") +
                      "}";
      }
      return "{" + output.str().substr(2) + "}";
    }
  }

  auto begin() const noexcept { return values.begin(); }
  auto end() const noexcept { return values.end(); }
  auto cend() const noexcept { return values.cend(); }

  bool operator==(const NestedVector<Type> &other) const {
    return values == other.values;
  }

  bool operator!=(const NestedVector<Type> &other) const {
    return !(*this == other);
  }

  friend ostream &operator<<(ostream &stream, const NestedVector &item) {
    return stream << item.str();
  }

  void push_back(const PrintableVector<Type> &value) {
    values.push_back(value);
  }
};

template <typename T>
string type_name();

template <class Type>
class PrintableOptional {
 private:
  variant<optional<Type>, optional<pair<Type, Type>>> value;
  //  optional<Type> value{std::nullopt};

 public:
  PrintableOptional() = delete;
  PrintableOptional(const optional<Type> value) : value{value} {}
  PrintableOptional(const optional<pair<Type, Type>> value) : value{value} {}
  virtual ~PrintableOptional() = default;

  virtual string str() const noexcept {
    sstream result;
    if (holds_alternative<optional<pair<Type, Type>>>(value)) {
      if (const auto &item = get<1>(value); item)
        result << (*item).first << ";" << (*item).second;
      else
        result << "None";
    } else {
      if (const auto &item = get<0>(value); item) {
        //        if constexpr (is_same_v<Type, int>)
        //          cout << "INT: " << *item << endl;
        //        else if constexpr (is_same_v<Type, double>)
        //          //        result << setprecision(6) << *item;
        //          cout << "DOUBLE: " << *item << endl;
        //        else
        //          cout << "OTHER: " << *item << endl;
        result << setprecision(6) << fixed << *item;
      } else
        result << "None";
    }
    return result.str();
  }

  friend ostream &operator<<(ostream &stream, const PrintableOptional &item) {
    return stream << item.str();
  }

  bool operator==(const PrintableOptional<Type> &other) {
    return this->value == other.value;
  }
};

template <class Key, class Value>
using values_map = std::unordered_map<Key, Value>;

class PrintableStrMap {
 private:
  values_map<string, opt_str> items{};
  vector<string> keys{};

 public:
  PrintableStrMap() = default;
  PrintableStrMap(const string &source, char names = ';', char values = '=') {
    map_fields(source, names, values);
  }

  auto begin() const noexcept { return items.begin(); }
  auto end() const noexcept { return items.end(); }
  auto cbegin() const noexcept { return items.cbegin(); }
  auto cend() const noexcept { return items.cend(); }

  vector<string> &get_keys() noexcept { return this->keys; }
  auto keys_begin() noexcept { return keys.begin(); }
  auto keys_end() noexcept { return keys.end(); }
  auto keys_cbegin() const noexcept { return keys.cbegin(); }
  auto keys_cend() const noexcept { return keys.cend(); }

  auto at(const string &key) const { return items.at(key); }
  auto &operator[](const string &key) { return items[key]; }
  std::optional<opt_str> get(const string &key) const noexcept {
    if (const auto found = items.find(key); found != end())
      return found->second;
    else
      return std::nullopt;
  }

  auto size() const { return keys.size(); }
  auto isEmpty() const { return keys.empty(); }

  string get(const string &key, const string &value,
             const string &empty = "") const {
    if (auto result = get(key))
      return (*result).value_or(empty);
    else
      return value;
  }

  bool has(const string &key) const {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
  }

  void map_fields(const string &source, char names = ';', char values = '=') {
    if (!source.size()) return;

    for (auto ele : StringDecompose::str_split(source, names, true)) {
      const auto mark(ele.find(values));

      string key{ele.substr(0, mark)};
      auto value{string::npos == mark ? opt_str{} : ele.substr(mark + 1)};

      if (auto [it, inserted] = items.try_emplace(key, value); !inserted) {
        if (auto value = (*it).second)
          throw runtime_error("Key " + key + "already in map -> " + *value);
        else
          throw runtime_error("Key " + key + "already in map -> None");
      } else {
        keys.emplace_back((*it).first);
      }
    }
  }

  string join_fields(bool ordered = true, char names = ';',
                     char values = '=') const {
    if (items.empty()) return "";

    sstream output;

    if (ordered) {
      for (const auto &key : keys) {
        output << names << key;
        if (const auto &value = items.at(key)) output << values << *value;
      }
    } else {
      for (const auto &[key, value] : items) {
        output << names << key;
        if (value) output << values << *value;
      }
    }

    return output.str().substr(1);
  }

  template <class It>
  string join_fields(It begin, It end, char names = ';', char values = '=',
                     std::function<string(const string &)> modify =
                         [](const string &ele) { return ele; }) const {
    if (items.empty()) return "";

    sstream output;

    for (auto key = begin; key != end; ++key) {
      if (const auto item = this->get(*key)) {
        output << names << *key;
        if (const auto &value = *item)
          if (value) output << values << modify(*value);
      }
    }

    return output.str().substr(1);
  }

  string str() const { return join_fields(); }

  friend ostream &operator<<(ostream &stream, const PrintableStrMap &item) {
    return stream << item.str();
  }
};
}  // namespace AGizmo::Printable
