#pragma once

#include "evaluation.h"
#include "strings.h"

#include <sstream>

using sstream = std::stringstream;

using namespace AGizmo;
using namespace Evaluation;
using namespace StringFormat;

using std::pair;
using pair_bool = pair<bool, bool>;
using pair_char = pair<char, char>;
using pair_int = pair<int, int>;

template <typename It>
Stats test_XOR(It begin, It end, sstream &message);

template <typename Type>
struct PrintableVector {
  vector<Type> value{};

  PrintableVector() = default;
  PrintableVector(std::initializer_list<Type> input) : value{input} {}
  template <class InputIt>
  PrintableVector(InputIt first, InputIt last) {
    value = vector<Type>(first, last);
  }

  string str() const {
    if (value.empty()) return "{}";

    return "{" + str_join(value.begin(), value.end(), ",") + "}";
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
        output << ", {" + str_join(ele.begin(), ele.end(), ",") + "}";
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

class XORWithBool : public BaseTest {
 private:
  using Input = pair_bool;
  using Output = bool;

  const Input input;
  const Output expected;
  Output outcome;

 public:
  XORWithBool(Input input, Output expected);

  string str() const noexcept {
    return "(" + to_string(input.first) + ", " + to_string(this->input.second) +
           ")\n Outcome: " + to_string(outcome) +
           "\nExpected: " + to_string(expected);
  }

  void validate() {
    outcome = Basic::XOR(input.first, input.second);
    this->status = outcome == expected;
  }
};

class XORWithChar : public BaseTest {
 private:
  using Input = pair_char;
  using Output = bool;

  const Input input;
  const Output expected;
  Output outcome;

 public:
  XORWithChar(Input input, Output expected);

  string str() const noexcept {
    return "(" + to_string(input.first) + ", " + to_string(this->input.second) +
           ")\n Outcome: " + to_string(outcome) +
           "\nExpected: " + to_string(expected);
  }

  void validate() {
    outcome = Basic::XOR(input.first, input.second);
    this->status = outcome == expected;
  }
};

class SplitVectorWithVectorSep : public BaseTest {
 private:
  using Input = PrintableVector<int>;
  using Sep = Input;
  using Output = NestedVector<int>;

  const Input input;
  const Sep sep;
  const Output expected;
  Output outcome;

 public:
  SplitVectorWithVectorSep(Input input, Sep sep, Output expected);

  string str() const noexcept {
    return "(" + this->input.str() + ", " + sep.str() +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }

  void validate() {
    Basic::split<Input, defs>(this->input.begin(), this->input.end(),
                              this->sep.begin(), this->sep.end(),
                              back_inserter(this->outcome.values));
    this->status = outcome == expected;
  }
};

class SplitVectorWithSep : public BaseTest {
 private:
  using Input = PrintableVector<int>;
  using Sep = int;
  using Output = NestedVector<int>;

  const Input input;
  const Sep sep;
  const Output expected;
  Output outcome;

 public:
  SplitVectorWithSep(Input input, Sep sep, Output expected);

  string str() const noexcept {
    return "(" + this->input.str() + ", " + to_string(sep) +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }

  void validate() {
    Basic::split<Input>(this->input.begin(), this->input.end(), this->sep,
                        back_inserter(this->outcome.values));
    this->status = outcome == expected;
  }
};
