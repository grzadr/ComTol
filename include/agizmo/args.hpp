#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <agizmo/strings.hpp>

namespace AGizmo::Args {

using std::nullopt;
using std::optional;
using std::runtime_error;
using std::string;
using std::to_string;
using std::vector;
using sstream = std::stringstream;

enum class ValueType { Bool = 1, Integer = 2, String = 3, Double = 4 };

inline string str_value_type(const ValueType &value_type) {
  switch (value_type) {
    case ValueType::Bool:
      return "Bool";
    case ValueType::Integer:
      return "Integer";
    case ValueType::String:
      return "String";
    case ValueType::Double:
      return "Double";
    default:
      throw runtime_error{"Unknown"};
  }
}

class Flag {
 private:
  int position;
  string name;
  string help;
  ValueType value_type;
  char alt_name;
  optional<string> value;

 public:
  Flag() = delete;
  Flag(int position, const string &name, const string &help,
       ValueType value_type, char alt_name, optional<string> default_val)
      : position{position},
        name{name},
        help{help},
        value_type{value_type},
        alt_name{alt_name},
        value{default_val} {}

  friend bool operator<(const Flag &lhs, const Flag &rhs) {
    if (lhs.position == 0)
      return false;
    else if (rhs.position == 0)
      return true;
    else
      return lhs.position < rhs.position;
  }

  bool hasValue() const { return value.has_value(); }
  bool isSet() const { return value.has_value(); }
  bool isEmpty() const { return !hasValue(); }
  bool isPositional() const { return position > 0; }
  bool isLogical() const { return value_type == ValueType::Bool; }
  auto getName() const noexcept { return name; }
  auto getAltName() const noexcept { return alt_name; }
  auto getPosition() const { return position; }
  auto getValueType() const { return value_type; }
  void reset() { value = nullopt; }
  auto getValue() const { return value; }
  auto getValue(const string &backup) const { return value.value_or(backup); }
  auto setValue(const string &value) { this->value = value; }

  string str() const {
    sstream output;
    if (isPositional())
      output << "@" << to_string(getPosition()) << " ";
    else
      output << "--";
    output << getName();

    if (alt_name) output << "/" << alt_name;

    output << ":" << str_value_type(getValueType()) << " -> ";
    if (isLogical())
      output << getValue("Off");
    else
      output << getValue("None");

    return output.str();
  }

  friend std::ostream &operator<<(std::ostream &stream, const Flag &item) {
    return stream << item.str();
  }
};

class Arguments {
 private:
  //  using alt_map = std::unordered_map<char, string>;
  //  using args_map = std::unordered_map<string, Flag>;

  vector<Flag> args{};

  void addArgument(int position, const string &name, const string &help,
                   const ValueType &value_type, char alt_name,
                   const optional<string> &def_value) {
    if (name.empty()) throw runtime_error{"Argument name cannot be empty!"};
    args.emplace_back(position, name, help, value_type, alt_name, def_value);
    std::stable_sort(args.begin(), args.end());
  }

 public:
  Arguments() = default;

  void addArgument(int position, const string &name, const string &help,
                   const ValueType &value_type) {
    if (position < 1)
      throw runtime_error{"Argument position must be greater than 0"};
    if (value_type == ValueType::Bool)
      throw runtime_error{"Can't set positional argument '" + name +
                          "' with type Bool"};
    addArgument(position, name, help, value_type, 0, nullopt);
  }

  void addArgument(const string &name, const string &help,
                   const ValueType &value_type, char alt_name = 0,
                   optional<string> def_value = nullopt) {
    addArgument(0, name, help, value_type, alt_name, def_value);
  }

  auto begin() const { return this->args.begin(); }
  auto cbegin() const { return this->args.cbegin(); }
  auto end() const { return this->args.end(); }
  auto cend() const { return this->args.cend(); }

  auto size() const { args.size(); }
  auto empty() const { args.empty(); }

  auto &getArg(const string &name) {
    for (auto &ele : args) {
      if (ele.getName() == name) return ele;
    }

    throw runtime_error("Argument not found:" + name);
  }

  auto &getArg(const int position) {
    for (auto &ele : args) {
      if (auto arg_pos = ele.getPosition(); arg_pos == position) return ele;
    }

    throw runtime_error("Positional argument not found:" + to_string(position));
  }

  optional<string> getArgName(const char alt_name) {
    for (auto &ele : args) {
      if (auto arg_alt_name = ele.getAltName(); arg_alt_name == alt_name)
        return ele.getName();
    }

    return nullopt;
  }

  void parse(int argc, char *argv[]) {
    int last_poz = 0;
    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];
      std::cerr << temp << "\n";
      if (temp == "--") {
        for (int j = i + 1; j < argc; ++j)
          this->getArg(++last_poz).setValue(argv[j]);
      } else if (temp == "-")
        throw runtime_error{"Missing option after '-' in position " +
                            to_string(i)};
      else if (StringSearch::str_starts_with(temp, "--")) {
        auto &arg_ref = getArg(temp.substr(2));
        if (arg_ref.isLogical())
          arg_ref.setValue("On");
        else if (++i == argc)
          throw runtime_error{"Missing value for argument " + temp};
        else
          arg_ref.setValue(argv[i]);
      } else if (temp.front() == '-') {
        if (temp = temp.substr(1); temp.size() == 1) {
          if (const auto &arg_name = getArgName(temp.front())) {
            auto &arg_ref = getArg(*arg_name);
            if (arg_ref.isLogical())
              arg_ref.setValue("");
            else if (++i == argc)
              throw runtime_error{"Missing value for argument " + temp};
            else
              arg_ref.setValue(argv[i]);
          } else {
            throw runtime_error{"Unknown argument " + to_string(temp.front())};
          }
        } else {
          //        string last_name = "";
          //        for (const auto &ele : temp) {
          //            if (auto arg_name = get)
          //        }
          throw runtime_error{
              "Multiple argument with '-' not supported. "
              "Contact lazy developer."};
        }
      } else
        this->getArg(++last_poz).setValue(argv[i]);
    }

    std::cerr << "Processed arguments\n";
    for (const auto &arg : args) std::cerr << arg << "\n";
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
};

}  // namespace AGizmo::Args
