#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std::string_literals;
// using namespace HKL;
// using namespace ComTol;
// using namespace ComTol::StrTol;

using std::cout;
using std::endl;
using std::pair;
using std::size_t;
using std::string;
using std::to_string;
using std::tuple;
using std::vector;

using sstream = std::stringstream;
using pair_int = pair<int, int>;
using pair_str = pair<string, string>;

namespace Evaluation {

struct Stats {
  int total{0}, failed{0};

  Stats &operator++() {
    ++total;
    return *this;
  }

  void operator()(const pair_int &stats) {
    total += stats.first;
    failed += stats.second;
  }

  void operator()(int t, int f) {
    total += t;
    failed += f;
  }

  void operator()(const Stats &input) {
    total += input.total;
    failed += input.failed;
  }

  int size() { return this->total; }

  explicit operator int() { return this->failed; }

  bool has_failed() { return this->failed != 0; }

  void add_fail(int f = 1) { failed += f; }

  pair_int get() const { return {total, failed}; }

  double get_ratio() const { return failed / static_cast<double>(total); }

//   friend std::ostream &operator<<(ostream &stream, const Stats &stats) {
//     return stream << stats.total;
//   }
};

inline const string passed_str{"[ PASSED ]"};
inline const string failed_str{"[~FAILED~]"};

inline string gen_framed(const string &message, size_t width = 80, char frame = '#'){

  string result{string(width, frame) + "\n"};
  string frame_string = string(3, frame);

  result += str_frame(message, width, frame_string, frame_string);

  return result + "\n" + string(width, frame);
}

inline string gen_pretty(const string &message, size_t width = 80){
  if (width % 2)
    ++width;

  string frame = string(width / 2, '<') + string(width / 2, '>');
  string result{frame};
  result += "\n" + str_frame(message, width, "<<<", ">>>");

  return result + "\n" + frame;
}

inline string gen_summary(int total, int failed, string type = "Evaluation")
{
  sstream message;

  message << type;

  if (failed)
    message << " failed in " << failed << "/" << total << " ("
            << std::setprecision(3) << 100.0 * failed / total << "%) cases!";
  else
    message << " succeeded in all " << total << " cases!";

  return message.str();
}

inline string gen_summary(const Stats &stats, string type = "Evaluation") {
  sstream message;

  message << type;

  if (stats.failed)
    message << " failed in " << stats.failed << "/" << stats.total << " ("
            << std::setprecision(5) << 100 * stats.get_ratio() << "%) cases!";
  else
    message << " succeeded in all " << stats.total << " cases!";

  return message.str();
}
int perform_tests(bool verbose = false);
} // namespace EvalTools
