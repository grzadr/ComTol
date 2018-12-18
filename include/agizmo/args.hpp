#pragma once

#include <optional>
#include <stdexcept>
#include <string>
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

  string name{};
  string help{};
  char alt_name{};
  optional<string> value{};
  int position = 0;

 public:
  Flag() = delete;
  Flag(const string& name, const string& help, char alt_name = 0,
       optional<string> def_value = nullopt, int position = 0)
      : name{name},
        help{help},
        alt_name{alt_name},
        value{def_value},
        position{position} {}

  bool hasValue() const { return value.has_value(); }
  bool isEmpty() const { return !hasValue(); }
  bool isPositional() const { return name.front() == '_'; }
  auto getName() const noexcept { return name; }
  auto getAltName() const noexcept { return alt_name; }
  auto getPosition() const { return std::stoi(name.substr(1)); }
  auto getValue() const { return value; }
  auto getValue(const string& backup) const { return value.value_or(backup); }
};

class Arguments {
 private:
  vector<Flag> args{};

 public:
  Arguments() = default;

  void addArgument(const string& name, const string& help, char alt_name = 0,
                   int position = 0) {
    addArgument(name, help, alt_name, nullopt, position);
  }

  void addArgument(const string& name, const string& help, char alt_name = 0,
                   optional<string> def_value = nullopt, int position = 0) {
    args.emplace_back(name, help, alt_name, def_value, position);
  }

  void parse(int argc, char* argv[]) {
    string name{};
    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];

      if (name.empty()) {
        if (temp.front() == '-')
          name = temp;
        else
          args.emplace_back(temp, i);
      } else {
        if (temp.front() == '-') {
          args.emplace_back(name);
          name = temp;
        } else {
          args.emplace_back(name, temp);
          name = {};
        }
      }
    }

    if (!name.empty()) args.emplace_back(name);
  }

  auto begin() const { return this->args.begin(); }
  auto cbegin() const { return this->args.cbegin(); }
  auto end() const { return this->args.end(); }
  auto cend() const { return this->args.cend(); }

  auto size() const { args.size(); }
  auto empty() const { args.empty(); }
};

}  // namespace AGizmo::Args
