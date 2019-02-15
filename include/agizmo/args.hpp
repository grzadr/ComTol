#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <agizmo/strings.hpp>

namespace AGizmo::Args {

using std::nullopt;
using std::optional;
using runerror = std::runtime_error;
using std::string;
using std::to_string;
using std::vector;
using sstream = std::stringstream;

using args_map = std::unordered_map<char, string>;

enum class ValueType { Switch = 1, Single = 2, Multiple = 3 };

inline string str_value_type(const ValueType &value_type) {
  switch (value_type) {
  case ValueType::Switch:
    return "Switch";
  case ValueType::Single:
    return "Single";
  case ValueType::Multiple:
    return "Multiple";
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
  int value_count;
  char alt_name;
  optional<string> value;
  bool obligatory;

public:
  Flag() = delete;
  Flag(int position, const string &name, const string &help,
       ValueType value_type, char alt_name, optional<string> default_val,
       bool obligatory)
      : position{position}, name{name}, help{help}, value_type{value_type},
        alt_name{alt_name}, value{default_val}, obligatory{obligatory} {}

  friend bool operator<(const Flag &lhs, const Flag &rhs) {
    if (lhs.position == 0)
      return false;
    else if (rhs.position == 0)
      return true;
    else
      return lhs.position < rhs.position;
  }

  bool hasValue() const { return value.has_value(); }
  bool isSet() const { return hasValue(); }
  bool isEmpty() const { return !hasValue(); }
  bool isPositional() const { return position > 0; }
  bool isSwitch() const { return value_type == ValueType::Switch; }
  bool isMultiple() const { return value_type == ValueType::Multiple; }
  bool isSingle() const { return value_type == ValueType::Single; }
  bool isObligatory() const { return obligatory; }
  bool isOptional() const { return !obligatory; }
  void makeObligatory() { obligatory = true; }
  void makeOptional() { obligatory = false; }
  auto getName() const noexcept { return name; }
  auto getAltName() const noexcept { return alt_name; }
  auto getPosition() const { return position; }
  auto makeMultiple() { this->value_type = ValueType::Multiple; }
  auto makeSingle() { this->value_type = ValueType::Single; }
  auto getValueType() const { return value_type; }
  void reset() { value = nullopt; }
  auto getValue() const {
    if (this->isSet())
      return *value;
    else
      throw runerror{"Argument " + name + " is not set!"};
  }
  auto getValue(const string &backup) const { return value.value_or(backup); }
  bool setValue(const string &value = "") {
    if (StringSearch::str_starts_with(value, "-"))
      throw runerror{"Missing value for " + name + "!"};
    else if (value_type == ValueType::Multiple) {
      this->value = this->value.value_or("") + string(1, 34) + value;
      return false;
    } else {
      bool result = this->value.has_value();
      this->value = value;
      return result;
    }
  }

  string str() const {
    sstream output;
    if (isPositional())
      output << "@" << to_string(getPosition()) << " ";
    else
      output << "--";
    output << getName();

    if (alt_name)
      output << "/" << alt_name;

    output << ":" << str_value_type(getValueType()) << " -> ";
    if (isSwitch())
      output << (hasValue() ? "On" : "Off");
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
  vector<Flag> args{};
  int last_position = 0;
  bool allow_overwrite{true};
  args_map alt_names_map;

  void addArgument(int position, const string &name, const string &help,
                   const ValueType &value_type, char alt_name,
                   const optional<string> &def_value, bool obligatory) {
    if (name.empty())
      throw runtime_error{"Argument name cannot be empty!"};
    if (contains(name))
      throw runerror("Argument '" + name + "' already added!");

    if (auto [it, inserted] = alt_names_map.try_emplace(alt_name, name);
        !inserted)
      throw runerror("Argument '" + string(1, alt_name) + "' already added!");

    args.emplace_back(position, name, help, value_type, alt_name, def_value,
                      obligatory);
  }

  auto &getArg(const string &name) {
    for (auto &ele : args) {
      if (ele.getName() == name)
        return ele;
    }

    throw runtime_error("Argument not found:" + name);
  }

  auto getArg(const string &name) const {
    for (auto &ele : args) {
      if (ele.getName() == name)
        return ele;
    }

    throw runtime_error("Argument not found:" + name);
  }

  auto &getArg(const int position) {
    for (auto &ele : args) {
      if (position == ele.getPosition())
        return ele;
    }

    throw runtime_error("Positional argument not found:" + to_string(position));
  }

  string getValue(int position) { return getArg(position).getValue(); }
  string getValue(int position, const string &default_value) {
    return getArg(position).getValue(default_value);
  }

public:
  Arguments() = default;
  Arguments(bool allow_overwrite) : allow_overwrite{allow_overwrite} {}

  void addPositional(const string &name, const string &help,
                     bool obligatory = false) {
    addArgument(++last_position, name, help, ValueType::Single, 0, nullopt,
                obligatory);
  }

  void addArgument(const string &name, const string &help, char alt_name,
                   optional<string> def_value = nullopt) {
    addArgument(0, name, help, ValueType::Single, alt_name, def_value, false);
  }

  void addArgument(const string &name, const string &help,
                   optional<string> def_value = nullopt) {
    addArgument(0, name, help, ValueType::Single, 0, def_value, false);
  }

  void addObligatory(const string &name, const string &help,
                     char alt_name = 0) {
    addArgument(0, name, help, ValueType::Single, alt_name, nullopt, true);
  }

  void addSwitch(const string &name, const string &help, char alt_name = 0) {
    addArgument(0, name, help, ValueType::Switch, alt_name, nullopt, false);
  }

  auto begin() const { return this->args.begin(); }
  auto cbegin() const { return this->args.cbegin(); }
  auto end() const { return this->args.end(); }
  auto cend() const { return this->args.cend(); }

  auto size() const { args.size(); }
  auto empty() const { args.empty(); }

  void makeObligatory(const string &name) { getArg(name).makeObligatory(); }
  void allowMultipleValues(const string &name) { getArg(name).makeMultiple(); }

  optional<string> getArgName(const char alt_name) {
    if (auto found = alt_names_map.find(alt_name); found != alt_names_map.end())
      return found->second;
    else
      return nullopt;
  }

  bool contains(const string &name) const {
    return std::find_if(begin(), end(), [&name](auto &a) {
             return a.getName() == name;
           }) != end();
  }

  bool isSet(const string &name) const { return getArg(name).isSet(); }

  string getValue(const string &name) const { return getArg(name).getValue(); }
  string getValue(const string &name, const string &default_value) const {
    return getArg(name).getValue(default_value);
  }

  string operator()(const string &name) const { return getValue(name); }
  string operator()(const string &name, const string &default_value) const {
    return getValue(name, default_value);
  }

  bool setValue(const string &name, const string &value = "") {
    if (auto overwrite = getArg(name).setValue(value);
        overwrite && !allow_overwrite)
      throw runerror("Argument " + name + "overwritten!");
    else
      return false;
  }
  bool setValue(int position, const string &value = "") {
    if (auto overwrite = getArg(position).setValue(value);
        overwrite && !allow_overwrite)
      throw runerror("Argument " + name + "overwritten!");
    else
      return false;
  }

  void parse(int argc, char *argv[]) {
    std::stable_sort(args.begin(), args.end());

    int current_position = 0;
    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];
      std::cerr << temp << "\n";
      // If single --  is encountered rest are
      // interpreted as positional arguments
      if (temp == "--") {
        for (; i < argc; ++i)
          setValue(++current_position, argv[i]);
      }
      //      TODO Should parser allow lonely '-' sign?
      //      else if (temp == "-")
      //        throw runtime_error{"Missing option after '-' in position " +
      //                            to_string(i)};
      // Argument starts with "--"
      else if (StringSearch::str_starts_with(temp, "--")) {
        if (auto arg_name = temp.substr(2);
            !StringSearch::contains(arg_name, '=')) {

          if (auto arg_ref = getArg(arg_name); arg_ref.isSwitch())
            arg_ref.setValue();
          else if (++i == argc)
            throw runtime_error{"Missing value for argument " + temp};
          else
            arg_ref.setValue(argv[i]);

        } else {
          auto [name, value] =
              StringDecompose::str_split_in_half(arg_name, '=');
          setValue(name, value);
        }

      } else if (temp.front() == '-') {
        if (temp = temp.substr(1); temp.size() == 1) {
          if (const auto &arg_name = getArgName(temp.front())) {
            auto &arg_ref = getArg(*arg_name);
            if (arg_ref.isSwitch())
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
          throw runtime_error{"Multiple argument with '-' not supported. "
                              "Contact lazy developer."};
        }
      } else
        this->getArg(++last_poz).setValue(argv[i]);
    }

    std::cerr << "Processed arguments\n";
    for (const auto &arg : args) {
      std::cerr << arg << "\n";
      if (arg.isObligatory() && arg.isEmpty())
        throw runtime_error{"Obligatory argument not set -> " + arg.str()};
    }
  }
};

} // namespace AGizmo::Args
