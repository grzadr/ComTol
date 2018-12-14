#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace AGizmo::Args {

using std::optional;
using std::runtime_error;
using std::string;
using std::vector;

class Flag {
 private:
  string name{};
  optional<string> value{};

 public:
  Flag() = delete;
  Flag(const string& name) {
    if (name.empty()) throw runtime_error("Name of the flag cannot be empty!");
    this->name = name.substr(name.find_first_not_of('-'));
  }
  Flag(const string& name, const string& value) : Flag(name) {
    this->value = value;
  }

  bool hasValue() const { return value.has_value(); }
  bool isEmpty() const { return !hasValue(); }
  bool isPositional() const { return name.front() == '_'; }
  auto getName() const { return name; }
  auto getPosition() const { return std::stoi(name.substr(1)); }
  auto getValue() const { return value; }
  auto getValue(const string& backup) const { return value.value_or(backup); }
};

class Arguments {
 private:
  vector<Flag> args{};

 public:
  Arguments() = delete;
  Arguments(int argc, char* argv[]) {
    string name{};
    int position = 0;
    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];

      if (name.empty()) {
        if (temp.front() == '-')
          name = temp;
        else {
          args.emplace_back(temp, i);
        }
        //          throw runtime_error{"Missing flag before '" + temp + "'"};
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
