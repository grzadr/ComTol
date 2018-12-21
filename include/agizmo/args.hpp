#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace AGizmo::Args {

using std::nullopt;
using std::optional;
using std::runtime_error;
using std::string;
using std::vector;

class Flag {
 private:
  static inline const string store_true{"_true"};
  static inline const string store_false{"_false"};

  int position;
  string name;
  string help;
  char alt_name;
  optional<string> value;

 public:
  Flag() = delete;
  Flag(int position, const string& name, const string& help, char alt_name,
       optional<string> def_value)
      : position{position},
        name{name},
        help{help},
        alt_name{alt_name},
        value{def_value} {}

  bool hasValue() const { return value.has_value(); }
  bool isEmpty() const { return !hasValue(); }
  bool isPositional() const { return position > 0; }
  auto getName() const noexcept { return name; }
  auto getAltName() const noexcept { return alt_name; }
  auto getPosition() const { return position; }
  auto getValue() const { return value; }
  auto getValue(const string& backup) const { return value.value_or(backup); }
};

class Arguments {
 private:
  using alt_map = std::unordered_map<char, string>;
  using args_map = std::unordered_map<string, Flag>;

  args_map args{};

  void addArgument(int position, const string& name, const string& help,
                   char alt_name, optional<string> def_value) {
    if (name.empty()) throw runtime_error{"Argument name cannot be empty!"};
    Flag temp_arg{position, name, help, alt_name, def_value};
    args.emplace(name, std::move(temp_arg));
  }

 public:
  Arguments() = default;

  void addArgument(int position, const string& name, const string& help) {
    if (position < 1)
      throw runtime_error{"Argument position must be greater than 0"};
    addArgument(position, name, help, 0, nullopt);
  }

  void addArgument(const string& name, const string& help, char alt_name,
                   optional<string> def_value = nullopt) {
    addArgument(0, name, help, alt_name, def_value);
  }

  void parse(int argc, char* argv[]) {
    //    string name{};
    //    for (int i = 1; i < argc; ++i) {
    //      string temp = argv[i];

    //      if (name.empty()) {
    //        if (temp.front() == '-')
    //          name = temp;
    //        else
    //          args.emplace_back(temp, i);
    //      } else {
    //        if (temp.front() == '-') {
    //          args.emplace_back(name);
    //          name = temp;
    //        } else {
    //          args.emplace_back(name, temp);
    //          name = {};
    //        }
    //      }
    //    }

    //    if (!name.empty()) args.emplace_back(name);
  }

  auto begin() const { return this->args.begin(); }
  auto cbegin() const { return this->args.cbegin(); }
  auto end() const { return this->args.end(); }
  auto cend() const { return this->args.cend(); }

  auto size() const { args.size(); }
  auto empty() const { args.empty(); }
};

}  // namespace AGizmo::Args
