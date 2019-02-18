#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
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
using std::cerr;
using opt_int = std::optional<int>;
using opt_str = std::optional<string>;

class BaseFlag {
private:
  string name;
  string help;
  char name_alt;
  opt_str value;
  bool obligatory;

public:
  BaseFlag() = default;
  BaseFlag(const string &name, const string &help, char name_alt,
           const opt_str &default_value, bool obligatory)
      : name{name}, help{help}, name_alt{name_alt}, value{default_value},
        obligatory{obligatory} {
    if (name.empty())
      throw runerror("Name of argument cannot be empty!");
    if (StringSearch::str_starts_with(name, "-"))
      throw runerror{"Name cannot not start wuth '-' sign"};
  }

  // Checkers

  bool isObligatory() const noexcept { return obligatory; }
  bool isSet() const { return value.has_value(); }

  // Getters

  string getName() const { return name; }
  char getNameAlt() const { return name_alt; }

  opt_str getValue() const { return value; }
  string getValue(const string &backup) const { return value.value_or(backup); }

  string getHelp() const { return help; }

  // Setters

  opt_str setValue(opt_str value = nullopt) {
    if (value.has_value() && StringSearch::str_starts_with(*value, "-"))
      throw runerror{"Value '" + *value + "' is not vali for argument '" +
                     name + "'"};
    std::swap(value, this->value);
    return value;
  }

  opt_str append(const string &value, char sep = 34) {
    if (this->value.has_value())
      *this->value += string(1, sep) + value;
    else
      this->value = value;

    return value;
  }

  opt_str reset() { return setValue(); }

  string str() const {
    return getName() + (name_alt ? "/" + string(1, name_alt) : "");
  }

  string str_help() const { return str(); }
};

class RegularFlag {
private:
  BaseFlag flag;

public:
  RegularFlag() = default;
  RegularFlag(const string &name, const string &help, char name_alt,
              const opt_str &default_value, bool obligatory)
      : flag{name, help, name_alt, default_value, obligatory} {}

  // Checkers

  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return flag.isObligatory(); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }
  opt_str getValue() const { return flag.getValue(); }
  opt_str getValue(const string &backup) const { return flag.getValue(backup); }

  // Setters

  opt_str setValue(const string &value) { return flag.setValue(value); }

  string str() const {
    sstream output;
    output << "R: " << flag.str()
           << " Value: " << std::quoted(flag.getValue().value_or("__Empty__"));
    return output.str();
  }
};

class PositionalFlag {
private:
  BaseFlag flag;
  int position;

public:
  PositionalFlag() = default;
  PositionalFlag(int position, const string &name, const string &help,
                 bool obligatory)
      : flag{name, help, 0, nullopt, obligatory} {
    if (position < 1)
      throw runerror{"Position must be an positive integer!"};
    else
      this->position = position;
  }

  // Checkers

  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return flag.isObligatory(); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return 0; }
  opt_str getValue() const { return flag.getValue(); }
  opt_str getValue(const string &backup) const { return flag.getValue(backup); }

  // Setters

  opt_str setValue(const string &value = "") { return flag.setValue(value); }

  string str() const {
    sstream output;
    output << "@" << position << " " << flag.str()
           << " Value: " << std::quoted(flag.getValue().value_or("__Empty__"));
    return output.str();
  }
};

class MultiFlag {
private:
  BaseFlag flag;

public:
  MultiFlag() = default;
  MultiFlag(const string &name, const string &help, char name_alt,
            bool obligatory)
      : flag{name, help, name_alt, nullopt, obligatory} {}

  //
  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return flag.isObligatory(); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }
  opt_str getValue() const { return flag.getValue(); }
  opt_str getValue(const string &backup) const { return flag.getValue(backup); }

  // Setters

  opt_str setValue(const string &value) { return flag.append(value); }

  string str() const {
    sstream output;
    output << "M: " << flag.str()
           << " Value: " << std::quoted(flag.getValue().value_or("__Empty__"));
    return output.str();
  }
};

class SwitchFlag {
private:
  BaseFlag flag;

public:
  SwitchFlag() = default;
  SwitchFlag(const string &name, const string &help, char name_alt)
      : flag{name, help, name_alt, nullopt, false} {}

  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return false; }

  opt_str setValue(const string &value = "") {
    //    std::cerr << "Setting switch " << this->getName() << "\n";
    return flag.setValue(value);
  }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }
  opt_str getValue() const { return flag.getValue(); }
  opt_str getValue(const string &backup) const { return flag.getValue(backup); }

  string str() const {
    sstream output;
    output << "S: " << flag.str() << " Value: "
           << std::quoted(flag.isSet() ? "__Set__" : "__NotSet__");
    return output.str();
  }
};

using Flags = std::variant<RegularFlag, PositionalFlag, MultiFlag, SwitchFlag>;
// using Flags = std::variant<PositionalFlag>;
using FlagsMap = std::unordered_map<string, Flags>;
using FlagsNamesAlt = std::unordered_map<char, string>;

class Arguments {
private:
  // Options
  bool allow_overwrite{true};

  // Arguments
  FlagsMap args{};
  opt_int numerical_arg;
  //  vector<Flags> args{};

  // Maps
  FlagsNamesAlt alt_names_map;
  vec_str positional_args;

  void insertPositional(const string &name) {
    cerr << "Inserting\n";
    positional_args.emplace_back(name);
  }

  template <class T, class... Args>
  void record(const string &name, Args &&... arguments) {
    if (const auto &[it, inserted] =
            args.try_emplace(name, std::in_place_type<T>, arguments...);
        !inserted)
      throw runerror{"Argument " + name + " alredy exists!"};
    else {
      std::visit(
          [this, &name](auto &&arg) {
            if (const auto name_alt = arg.getNameAlt(); name_alt) {
              if (const auto &[it, inserted] =
                      this->alt_names_map.try_emplace(name_alt, name);
                  !inserted)
                throw runerror{"Flag '" + string(1, name_alt) +
                               "' alredy bounded to argument " + it->second};
            }
          },
          it->second);

      if (std::holds_alternative<PositionalFlag>(it->second))
        insertPositional(name);
    }
  }

  bool _contains(const string &name) const {
    return args.find(name) != args.end();
  }

  opt_str _contains(const char name) const {
    if (auto it = alt_names_map.find(name); it != alt_names_map.end())
      return it->second;
    else
      return nullopt;
  }

  auto &getArg(const string &name) {
    if (_contains(name))
      return args[name];
    else
      throw runerror{"Argument " + name + " does not exist!"};
  }

  auto &getArg(const char name) {
    if (const auto arg_name = _contains(name))
      return args[*arg_name];
    else
      throw runerror{"Symbol '" + string(1, name) + "' is not valid flag!"};
  }

  auto &operator[](const string &name) { return args[name]; }

  optional<string> setValue(const string &name, const string &value = "") {
    return std::visit([&value](auto &&arg) { return arg.setValue(value); },
                      getArg(name));
  }

  optional<string> setValue(size_t position, const string &value = "") {
    if (position < positional_args.size())
      return setValue(positional_args[position], value);
    else
      throw runerror{"Argument with postion '" + to_string(position) +
                     "' was not added!"};
  }

  void parseGroup(const string &flag) {
    string last_arg = "";

    for (size_t flag_pos = 0; flag_pos < flag.size(); ++flag_pos) {
      if (const auto flag_name = _contains(flag[flag_pos])) {
        if (auto &arg = args[*flag_name];
            std::holds_alternative<SwitchFlag>(arg))
          std::get<SwitchFlag>(arg).setValue();
        else {
          if (const auto value = flag.substr(flag_pos + 1); value.empty())
            throw runerror{"Missing value for " + *flag_name + " !"};
          else
            std::visit([&value](auto &&arg) { arg.setValue(value); }, arg);
        }
      } else if (const auto number = StringFormat::str_to_int(flag))
        numerical_arg = *number;
      else
        throw runerror{"Could not recognize one of these flags: " + flag};
    }
  }

public:
  Arguments() = default;
  Arguments(bool allow_overwrite) : allow_overwrite{allow_overwrite} {}

  void addPositional(const string &name, const string &help,
                     bool obligatory = false) {
    record<PositionalFlag>(name, static_cast<int>(positional_args.size() + 1),
                           name, help, obligatory);
  }

  void addArgument(const string &name, const string &help, char alt_name,
                   optional<string> def_value = nullopt) {
    record<RegularFlag>(name, name, help, alt_name, def_value, false);
  }

  void addArgument(const string &name, const string &help,
                   optional<string> def_value = nullopt) {
    record<RegularFlag>(name, name, help, 0, def_value, false);
  }

  void addObligatory(const string &name, const string &help,
                     char alt_name = 0) {
    record<RegularFlag>(name, name, help, alt_name, nullopt, true);
  }

  void addSwitch(const string &name, const string &help, char alt_name = 0) {
    record<SwitchFlag>(name, name, help, alt_name);
  }

  void addMulti(const string &name, const string &help, char alt_name = 0) {
    record<MultiFlag>(name, name, help, alt_name, false);
  }

  void addMultiObligatory(const string &name, const string &help,
                          char alt_name = 0) {
    record<MultiFlag>(name, name, help, alt_name, true);
  }

  auto begin() const { return this->args.begin(); }
  auto cbegin() const { return this->args.cbegin(); }
  auto end() const { return this->args.end(); }
  auto cend() const { return this->args.cend(); }

  auto size() const { args.size(); }
  auto empty() const { args.empty(); }

  auto getArg(const string &name) const {
    if (_contains(name))
      return args.at(name);
    else
      throw runerror{"Argument " + name + " does not exist!"};
  }

  auto isSet(const string &name) const {
    return std::visit([](auto &&arg) { return arg.isSet(); }, getArg(name));
  }

  auto getValue(const string &name) const {
    return std::visit([](auto &&arg) { return arg.getValue(); }, getArg(name));
  }

  auto getValue(const string &name, const string &backup) const {
    return getValue(name).value_or(backup);
  }

  auto operator()(const string &name) const { return getValue(name); }

  auto operator()(const string &name, const string &backup) const {
    return getValue(name, backup);
  }

  auto iterateValues(const string &name) const {

    return std::visit(
        [](auto &&arg) {
          if (!arg.isSet())
            throw runerror{"Argument " + arg.getName() + " is not set!"};
          return StringDecompose::str_split(*arg.getValue(), 34, true);
        },
        getArg(name));
  }

  string str() const {
    sstream output;

    output << "Arguments:\n";

    for (const auto &[key, arg] : args)
      std::visit([&output](auto &arg) { output << arg.str() << "\n"; }, arg);

    return output.str();
  }

  bool parse(int argc, char *argv[]) {
    size_t current_position = 0;

    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];

      // If single --  is encountered remainging flags are
      // interpreted as positional arguments
      if (temp == "--") {
        for (; i < argc; ++i, current_position++)
          setValue(current_position++, argv[i]);
      } else if (temp == "-")
        throw runtime_error{"Missing option after '-' in position " +
                            to_string(i)};
      // Argument starts with "--"
      else if (StringSearch::str_starts_with(temp, "--")) {
        if (auto arg_name = temp.substr(2);
            !StringSearch::contains(arg_name, '=')) {
          if (auto &arg = getArg(arg_name);
              std::holds_alternative<SwitchFlag>(arg)) {
            std::get<SwitchFlag>(arg).setValue();
          } else if (++i == argc)
            throw runtime_error{"Missing value for argument " + temp};
          else
            std::visit([argv, i](auto &&arg) { arg.setValue(argv[i]); }, arg);
        } else {
          auto [name, value] =
              StringDecompose::str_split_in_half(arg_name, '=');
          setValue(name, value);
        }
      } else if (temp.front() == '-') {
        if (auto arg_name = temp.substr(1);
            !StringSearch::contains(arg_name, '=')) {
          if (arg_name.size() > 1) {
            parseGroup(arg_name);
          } else {
            if (auto &arg = getArg(arg_name.front());
                std::holds_alternative<SwitchFlag>(arg)) {
              std::get<SwitchFlag>(arg).setValue();
            } else if (++i == argc)
              throw runtime_error{"Missing value for argument " + temp};
            else
              std::visit([argv, i](auto &&arg) { arg.setValue(argv[i]); }, arg);
          }
        } else {
          auto [name, value] =
              StringDecompose::str_split_in_half(arg_name, '=');
          if (name.size() > 1)
            throw runerror{"Using assignment symbol '=' and joining flags is "
                           "forbidden! -> " +
                           arg_name};
          else if (auto &arg = getArg(name.front());
                   std::holds_alternative<SwitchFlag>(arg))
            throw runerror{"Switch '" + name +
                           "' cannot have value assigned to it!"};
          else
            std::visit([&value](auto &&arg) { arg.setValue(value); }, arg);
        }
      } else
        setValue(current_position++, argv[i]);
    }

    int check = 0;

    cerr << "Processed arguments\n";
    for (const auto &[arg_name, arg] : args) {
      std::visit(
          [&check](auto &&arg) {
            cerr << arg.str() << "\n";
            if (arg.isObligatory() && !arg.isSet()) {
              check++;
              cerr << "Obligatory argument not set -> " + arg.getName() << "\n";
            }
          },
          arg);
    }

    return check != 0;
  }
}; // namespace AGizmo::Args

} // namespace AGizmo::Args
