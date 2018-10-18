#pragma once

#include <iomanip>
#include <iostream>
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

template <typename T> string type_name();

template <class Type> class PrintableOptional {
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
} // namespace AGizmo::Printable
