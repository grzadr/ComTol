#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "strings.h"

using namespace std::string_literals;
// using namespace HKL;
// using namespace ComTol;
// using namespace ComTol::StrTol;

using std::cout;
using std::endl;
using std::pair;

using std::back_inserter;
using std::tuple;
using std::vector;

using std::ostream;
using std::size_t;
using std::string;
using std::to_string;
using sstream = std::stringstream;
using std::left;
using std::right;
using std::setfill;
using std::setw;

using std::transform;

using std::log10;

// using pair_int = pair<int, int>;
using pair_str = pair<string, string>;

namespace AGizmo {

using StringFormat::str_frame;

namespace Evaluation {

class Stats {
 private:
  int total{0}, failed{0};

 public:
  Stats() = default;
  Stats(int total, int failed = 0) : total{total}, failed{0} {}

  Stats &operator++() {
    ++total;
    return *this;
  }

  void update(int t, int f) {
    total += t;
    failed += f;
  }

  void update(const Stats &input) { this->update(input.total, input.failed); }

  void operator()(int t, int f) { this->update(t, f); }

  void operator()(const Stats &input) { this->update(input); }

  void reset() {
    total = 0;
    failed = 0;
  }

  int size() const { return this->total; }

  explicit operator int() const { return this->failed; }

  int getFailed() const { return this->failed; }
  bool hasFailed() const { return this->failed != 0; }
  void addFailure(int f = 1) { failed += f; }

  double getRatio() const { return failed / static_cast<double>(total); }

  friend std::ostream &operator<<(ostream &stream, const Stats &stats) {
    return stream << stats.total;
  }
};

// template <typename OutType>
// class Output {
// protected:
//  OutType output{};
// public:
//  Output() = default;
//  virtual ~Output() = default;
//  explicit Output(OutType output): output{move(output)} {};
//  virtual string str() const = 0;
//};

// template <class Value>
// class BaseOutput {
// protected:
//  Value values;

// public:
//  virtual ~BaseOutput() = default;
//  virtual string str() const = 0;
//};

class BaseTest {
 protected:
  bool status;

 public:
  BaseTest() = default;
  virtual ~BaseTest() = default;
  virtual string str() const = 0;
  friend ostream &operator<<(ostream &stream, const BaseTest &item) {
    return stream << item.str();
  }

  virtual void validate() = 0;
  bool valid() const { return status; }
  operator int() const { return this->valid(); }
};

template <class Test>
class Evaluator {
 private:
  const string passed_str{"[ PASSED ]"};
  const string failed_str{"<<FAILED>>"};
  string name;
  const vector<Test> &tests;
  Stats result;

 public:
  Evaluator() = delete;
  ~Evaluator() = default;
  Evaluator(string name, const vector<Test> &tests, Stats result = {})
      : name{name}, tests{tests}, result{result} {}

  Stats verify(sstream &message) {
    auto number_width = static_cast<int>(log10(tests.size())) + 1;

    for (const auto &test : tests) {
      message << "Test " << setw(number_width) << setfill('0') << right
              << ++result << " ";
      result.addFailure(!test);

      message << setfill(' ') << setw(76 - number_width - 2);

      if (test)
        message << passed_str << "\n";
      else
        message << failed_str << "\nFunction: " << name << test << "\n";
    }

    return result;
  }

  Stats verify() {
    for_each(tests.begin(), tests.end(),
             [this](const Test &test) { this->result.addFailure(test); });

    return result;
  }
};

inline const string passed_str{"[ PASSED ]"};
inline const string failed_str{"[~FAILED~]"};

inline string gen_framed(const string &message, size_t width = 80,
                         char frame = '#') {
  string result{string(width, frame) + "\n"};
  string frame_string = string(3, frame);

  result += str_frame(message, width, frame_string, frame_string);

  return result + "\n" + string(width, frame);
}

inline string gen_pretty(const string &message, size_t width = 80) {
  if (width % 2) ++width;

  string frame = string(width / 2, '<') + string(width / 2, '>');
  string result{frame};
  result += "\n" + str_frame(message, width, "<<<", ">>>");

  return result + "\n" + frame;
}

inline string gen_summary(const Stats &stats, string type = "Evaluation",
                          bool framed = false) {
  sstream message;

  message << type;

  if (stats.getFailed())
    message << " failed in " << stats.getFailed() << "/" << stats.size() << " ("
            << std::setprecision(5) << 100 * stats.getRatio() << "%) cases!";
  else
    message << " succeeded in all " << stats.size() << " cases!";

  return framed ? gen_framed(message.str()) : message.str();
}

}  // namespace Evaluation

}  // namespace AGizmo
