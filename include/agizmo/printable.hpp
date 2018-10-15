#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <agizmo/strings.hpp>

namespace AGizmo::Printable {
using std::optional;
using std::ostream;
using std::string;
using std::vector;

template <typename Type> struct PrintableVector {
  vector<Type> value{};

  PrintableVector() = default;
  PrintableVector(std::initializer_list<Type> input) : value{input} {}
  template <class InputIt> PrintableVector(InputIt first, InputIt last) {
    value = vector<Type>(first, last);
  }
  PrintableVector(vector<Type> input) : value{input} {}

  string str() const {
    if (value.empty())
      return "{}";

    return "{" + StringCompose::str_join(value.begin(), value.end(), ",") + "}";
  }

  auto begin() const noexcept { return value.begin(); }
  auto end() const noexcept { return value.end(); }
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

template <typename Type = int> struct NestedVector {
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

template <class Type> struct PrintableOptional {
  std::optional<Type> value{std::nullopt};

  string str() const noexcept {
    return value == std::nullopt ? "std::nullopt" : to_string(*value);
  }

  friend ostream &operator<<(ostream &stream, const PrintableOptional &item) {
    return stream << item.str();
  }

  PrintableOptional() = default;
  PrintableOptional(const std::optional<Type> value) : value{value} {}

  bool operator==(const PrintableOptional<Type> &other) {
    return this->value == other.value;
  }
};
} // namespace AGizmo::Printable
