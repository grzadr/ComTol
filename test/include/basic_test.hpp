#pragma once

#include "agizmo/basic.hpp"
#include "agizmo/evaluation.hpp"
#include "agizmo/files.hpp"
#include "agizmo/strings.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

using sstream = std::stringstream;

using namespace AGizmo;
using namespace Evaluation;
// using namespace StringFormat;
// using namespace StringForm;

using std::pair;
using pair_bool = pair<bool, bool>;
using pair_char = pair<char, char>;
using pair_int = pair<int, int>;

template <typename It> Stats test_XOR(It begin, It end, sstream &message);

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

class XORWithBool : public BaseTest<pair_bool, bool> {
public:
  XORWithBool(pair_bool input, bool expected);

  string str() const noexcept {
    return "(" + to_string(input.first) + ", " + to_string(this->input.second) +
           ")\n Outcome: " + to_string(outcome) +
           "\nExpected: " + to_string(expected);
  }

  bool validate() {
    outcome = Basic::XOR(input.first, input.second);
    return this->setStatus(outcome == expected);
  }
};

class XORWithChar : public BaseTest<pair_char, bool> {
public:
  XORWithChar(pair_char input, bool expected);

  string str() const noexcept {
    return "(" + to_string(input.first) + ", " + to_string(this->input.second) +
           ")\n Outcome: " + to_string(outcome) +
           "\nExpected: " + to_string(expected);
  }

  bool validate() {
    outcome = Basic::XOR(input.first, input.second);
    return this->setStatus(outcome == expected);
  }
};

template <class Sep> struct InputVectorSep {
  const PrintableVector<int> elements;
  const Sep sep;
};

class SplitVectorWithVectorSep
    : public BaseTest<InputVectorSep<PrintableVector<int>>, NestedVector<int>> {
public:
  SplitVectorWithVectorSep(InputVectorSep<PrintableVector<int>> input,
                           NestedVector<int> expected);

  string str() const noexcept {
    return "(" + this->input.elements.str() + ", " + input.sep.str() +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }

  bool validate() {
    Basic::split<PrintableVector<int>, defs>(
        this->input.elements.begin(), this->input.elements.end(),
        this->input.sep.begin(), this->input.sep.end(),
        back_inserter(this->outcome.values));
    return this->setStatus(outcome == expected);
  }
};

class SplitVectorWithSep
    : public BaseTest<InputVectorSep<int>, NestedVector<int>> {
public:
  SplitVectorWithSep(InputVectorSep<int> input, NestedVector<int> expected);

  string str() const noexcept {
    return "(" + this->input.elements.str() + ", " + to_string(input.sep) +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }

  bool validate() {
    Basic::split<PrintableVector<int>>(
        this->input.elements.begin(), this->input.elements.end(),
        this->input.sep, back_inserter(this->outcome.values));
    return this->setStatus(outcome == expected);
  }
};

struct SegmentInput {
  PrintableVector<int> elements;
  int length;
};

class SegmentVector : public BaseTest<SegmentInput, NestedVector<int>> {
public:
  SegmentVector(SegmentInput input, NestedVector<int> expected);

  bool validate() {
    Basic::segment<PrintableVector<int>>(
        input.elements.begin(), input.elements.end(),
        back_inserter(this->outcome.values), input.length);
    return this->setStatus(outcome == expected);
  }

  string str() const noexcept {
    return "(" + this->input.elements.str() + ", " + to_string(input.length) +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }
};

class MergeVector : public BaseTest<InputVectorSep<int>, PrintableVector<int>> {
public:
  MergeVector(InputVectorSep<int> input, PrintableVector<int> expected);
  string str() const noexcept {
    return "(" + this->input.elements.str() + ", " + to_string(input.sep) +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }
  bool validate() {
    Basic::merge(input.elements.begin(), input.elements.cend(),
                 back_inserter(outcome.value), input.sep);
    return this->setStatus(outcome == expected);
  }
};

class OnlyDigits : public BaseTest<string, bool> {
public:
  OnlyDigits(string input, bool expected);

  string str() const noexcept {
    return "(" + this->input + ")\n Outcome: " + to_string(outcome) +
           "\nExpected: " + to_string(expected);
  }

  bool validate() {
    outcome = StringFormat::only_digits(input);
    return this->setStatus(outcome == expected);
  }
};

class StrToInt : public BaseTest<string, PrintableOptional<int>> {
public:
  StrToInt(string input, PrintableOptional<int> expected);

  string str() const noexcept {
    return "(" + this->input + ")\n Outcome: " + outcome.str() +
           "\nExpected: " + expected.str();
  }

  bool validate() {
    outcome = PrintableOptional(StringFormat::str_to_int(input));
    return this->setStatus(outcome == expected);
  }
};

class StrCleanEnds : public BaseTest<string, string> {
public:
  StrCleanEnds(string input, string expected);

  string str() const noexcept {
    return "(" + this->input + ")\n Outcome: " + outcome +
           "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringFormat::str_clean_ends(input);
    return this->setStatus(outcome == expected);
  }
};

struct StrCleanWithCharsInput {
  string query, chars;
};

class StrCleanEndsWithChars : public BaseTest<StrCleanWithCharsInput, string> {
public:
  StrCleanEndsWithChars(StrCleanWithCharsInput input, string expected);

  string str() const noexcept {
    return "(" + this->input.query + ", " + input.chars +
           ")\n Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringFormat::str_clean_ends(input.query, input.chars);
    return this->setStatus(outcome == expected);
  }
};

class StrClean : public BaseTest<string, string> {
public:
  StrClean(string input, string expected);

  string str() const noexcept {
    return "(" + this->input + ")\n Outcome: " + outcome +
           "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringFormat::str_clean(input);
    return this->setStatus(outcome == expected);
  }
};

class StrCleanWithChars : public BaseTest<StrCleanWithCharsInput, string> {
public:
  StrCleanWithChars(StrCleanWithCharsInput input, string expected);

  string str() const noexcept {
    return "(" + this->input.query + ", " + input.chars +
           ")\n Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringFormat::str_clean(input.query, true, input.chars);
    return this->setStatus(outcome == expected);
  }
};

class StrJoin : public BaseTest<PrintableVector<int>, string> {
public:
  StrJoin(PrintableVector<int> input, string expected);

  string str() const noexcept {
    return "(" + this->input.str() + ")\n Outcome: " + outcome +
           "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringCompose::str_join(input.begin(), input.end(), "-");
    return this->setStatus(outcome == expected);
  }
};

class StrReverse : public BaseTest<string, string> {
public:
  StrReverse(string input, string expected);

  string str() const noexcept {
    return "(" + this->input + ")\n Outcome: " + outcome +
           "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringFormat::str_reverse(input);
    return this->setStatus(outcome == expected);
  }
};

class StrSplit : public BaseTest<pair_str, PrintableVector<string>> {
public:
  StrSplit(pair_str input, PrintableVector<string> expected);

  string str() const noexcept {
    return "(" + this->input.first + "," + input.second +
           ")\n Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }

  bool validate() {
    outcome =
        PrintableVector(StringDecompose::str_split(input.first, input.second));
    return this->setStatus(outcome == expected);
  }
};

struct StrReplaceInput {
  string source, query, value;
};

class StrReplace : public BaseTest<StrReplaceInput, string> {
public:
  StrReplace(StrReplaceInput input, string expected);

  string str() const noexcept {
    return "(" + this->input.source + "," + input.query + "," + input.value +
           ")\n Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    outcome = StringFormat::str_replace(input.source, input.query, input.value);
    return this->setStatus(outcome == expected);
  }
};

class OpenFile : public BaseTest<string, string> {
public:
  OpenFile(string input, string expected);

  string str() const noexcept {
    return "(" + this->input + ")\n Outcome: " + outcome +
           "\nExpected: " + expected;
  }

  bool validate() {
    auto file = std::ofstream("test.txt");
    file << input << "\n";
    file.close();

    try {
      std::ifstream input{};
      Files::open_file("test.txt", input);
      getline(input, outcome);
    } catch (const std::runtime_error &ex) {
      std::cerr << ex.what();
    }

    return this->setStatus(outcome == expected);
  }
};
