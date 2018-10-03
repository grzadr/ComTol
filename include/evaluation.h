#pragma once

#include <algorithm>
#include <chrono>
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

using std::transform;

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

class BaseOutput {
public:
  BaseOutput() = default;
  virtual ~BaseOutput() = default;
  virtual string str() const = 0;
};

class BaseInput {
public:
  BaseInput() = default;
  virtual ~BaseInput() = default;
  virtual string str() const = 0;
  virtual BaseOutput &validate() const = 0;
};

template <typename Input, typename Output> class Evaluator {
  string name;
  //  const vector<BaseInput> &input;
  //  const vector<BaseOutput> &expected, outcome;
  const vector<Input> &input;
  const vector<Output> &expected;
  vector<Output> outcome;

private:
public:
  Evaluator() = delete;

  Evaluator(string name, const vector<Input> &input,
            const vector<Output> &expected)
      : name{name}, input{input}, expected{expected} {
    if (input.size() != expected.size())
      throw runtime_error("Input and Expected have different sizes!");
  }

  void generate() {
    transform(input.begin(), input.end(), back_inserter(outcome),
              [](const Input &ele) { return ele.validate(); });

    //    for (const auto& ele: outcome )
    //      cout << ele.str() << endl;
  }

  //  Stats verify(bool verbose=false){
  //    Stats result;
  //    for (auto outcome_it = outcome.begin(), expected_it = expected.begin();
  //         outcome_it != outcome.end; ++outcome_it, ++expected_it){
  //      ++result;
  //      if (*outcome_it != *expected_it)
  //        result.addFailure();
  //    }

  //    return result;
  //  }
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
  if (width % 2)
    ++width;

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

} // namespace Evaluation

} // namespace AGizmo
