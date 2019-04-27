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
  string name{};
  string help{};
  char name_alt{0};
  opt_str value{nullopt};
  bool obligatory{false};
  char sep{0};

public:
  BaseFlag() = default;
  BaseFlag(const string &name, const string &help, char name_alt,
           const opt_str &default_value, bool obligatory, char sep = 0)
      : name{name}, help{help}, name_alt{name_alt}, value{default_value},
        obligatory{obligatory}, sep{sep} {
    if (name.empty())
      throw runerror("Name of argument cannot be empty!");
    if (StringSearch::str_starts_with(name, "-"))
      throw runerror{"Name cannot not start wuth '-' sign"};
  }

  // Checkers

  bool isObligatory() const noexcept { return obligatory; }
  bool isSet() const { return this->value.has_value(); }
  bool isAppendable() const { return this->sep; }

  // Getters

  string getName() const { return name; }
  char getNameAlt() const { return name_alt; }

  opt_str getValue() const { return value; }
  string getValue(const string &backup) const { return value.value_or(backup); }

  string getHelp() const { return help; }

  int getValueCount() const {
    if (!this->isSet())
      return 0;
    else if (!this->isAppendable())
      return 1;
    else
      return static_cast<int>(StringSearch::count_all(*value, this->sep) + 1);
  }

  // Setters

  void makeObligatory() { this->obligatory = true; }
  void setSeparator(char sep = 0) { this->sep = sep; }
  void enableAppend(char sep) { this->setSeparator(sep); }
  void disableAppend() { this->setSeparator(0); }

  void setValue(const opt_str &value) { this->value = value; }

  void append(const string &value, char sep) {
    if (this->isSet())
      *this->value += string(1, sep) + value;
    else
      this->value = value;
  }

  void setValue(const string &value) {
    if (sep)
      this->append(value, sep);
    else
      this->setValue(value);
  }

  void reset() { setValue(nullopt); }

  string repr() const {
    return getName() + (name_alt ? "/" + string(1, name_alt) : "");
  }

  //  string str_help() const { return str(); }
};

class RegularFlag {
private:
  inline static const string kind{"Regular"};
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
  string getValue(const string &backup) const { return flag.getValue(backup); }
  string getKind() const { return this->kind; }

  // Setters

  void setValue(const string &value) { flag.setValue(value); }
  void enableAppend(char sep) { this->flag.enableAppend(sep); }
  void disableAppend() { this->flag.disableAppend(); }

  string repr() const {
    sstream output;
    output << "R: " << flag.repr()
           << " Value: " << std::quoted(flag.getValue().value_or("__Empty__"));
    return output.str();
  }
};

class MultiFlag {
private:
  inline static const string kind{"Multi"};
  BaseFlag flag;
  int lowest{0};
  int saturation{0};

public:
  MultiFlag() = default;
  MultiFlag(const string &name, const string &help, char name_alt, int lowest,
            int saturation)
      : flag{name, help, name_alt, nullopt, lowest > 0, 34} {
    this->setLowest(lowest);
    this->setSaturation(saturation);
  }

  //
  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return lowest; }
  bool isLoadable() const {
    return saturation < 1 || getValueCount() < saturation + 1;
  }
  bool isSaturated() const { return not this->isLoadable(); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }
  opt_str getValue() const { return flag.getValue(); }
  string getValue(const string &backup) const { return flag.getValue(backup); }
  int getValueCount() const { return flag.getValueCount(); }
  int getSaturation() const { return this->saturation; }
  string getKind() const { return this->kind; }

  // Setters

  void setSaturation(int saturation) {
    if (saturation < 0)
      throw runerror{"Saturation value must be at least 0 or greater."};
    else if (saturation < this->lowest)
      throw runerror{"Saturation " + std::to_string(saturation) +
                     " is lower than the lowest posiible value " +
                     std::to_string(lowest)};
    else
      this->saturation = saturation;
  }

  void setLowest(int lowest) {
    if (lowest < 0)
      throw runerror{"Lowest value must be at least 0 or greater."};
    else if (!this->saturation && lowest > this->saturation)
      throw runerror{"Lowest " + std::to_string(lowest) +
                     " value is higher than saturation " +
                     std::to_string(this->saturation)};
    else
      this->lowest = lowest;
  }

  //  opt_str setValue(const string &value) {
  void setValue(const string &value) { return flag.setValue(value); }

  string repr() const {
    sstream output;
    output << "MultiFlag: " << flag.repr()
           << " Value: " << std::quoted(flag.getValue().value_or("__Empty__"));
    return output.str();
  }
};

class PositionalFlag {
private:
  MultiFlag flag{};
  int position{1};
  inline const static string kind{"Positional"};

public:
  PositionalFlag() = default;
  PositionalFlag(int position, const string &name, const string &help,
                 int lowest, int saturation)
      : flag{name, help, 0, lowest, saturation}, position{position} {}

  // Checkers

  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return flag.isObligatory(); }
  bool isLoadable() const { return this->flag.isLoadable(); }
  bool isSaturated() const { return not this->flag.isSaturated(); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return 0; }
  opt_str getValue() const { return flag.getValue(); }
  string getValue(const string &backup) const { return flag.getValue(backup); }
  int getValueCount() const { return this->flag.getValueCount(); }
  string getKind() const { return this->kind; }

  // Setters

  void setValue(const string &value = "") { flag.setValue(value); }
  void setSaturation(int saturation) { this->flag.setSaturation(saturation); }
  void setLowest(int lowest) { this->flag.setLowest(lowest); }
  void setPosition(int position) {
    if (position < 1)
      throw runerror{"Position must be an positive integer!"};
    else
      this->position = position;
  }

  // Display

  string repr() const {
    sstream output;
    output << "@" << position << " " << flag.repr()
           << " Value: " << std::quoted(flag.getValue().value_or("__Empty__"));
    return output.str();
  }
};

class SwitchFlag {
private:
  BaseFlag flag;
  inline const static string kind = "Switch";

public:
  SwitchFlag() = default;
  SwitchFlag(const string &name, const string &help, char name_alt)
      : flag{name, help, name_alt, nullopt, false} {}

  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return false; }

  void setValue(const string &value = "") { return flag.setValue(value); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }
  opt_str getValue() const { return flag.getValue(); }
  string getValue(const string &backup) const { return flag.getValue(backup); }
  string getKind() const { return this->kind; }

  string repr() const {
    sstream output;
    output << "S: " << flag.repr() << " Value: "
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

  char help_flag_alt{'h'};
  string help_flag{"help"};

  // Arguments
  FlagsMap args{};
  opt_int numerical_arg;

  // Maps
  FlagsNamesAlt alt_names_map;
  vec_str positional_args;

  void insertPositional(const string &name) {
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

  void setValue(const string &name, const string &value = "") {
    if (auto &arg = this->getArg(name);
        std::holds_alternative<SwitchFlag>(arg) && value != "")
      throw runerror{"Switch '" + name + "' cannot have value assigned to it!"};
    else
      std::visit([&value](auto &&arg) { return arg.setValue(value); },
                 getArg(name));
  }

  void setValue(size_t position, const string &value = "") {
    if (position < positional_args.size())
      setValue(positional_args[position], value);
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

  void parseEqualSign(const string &arg) {
    auto [name, value] = StringDecompose::str_split_in_half(arg, '=');
    if (StringSearch::str_starts_with(name, "--"))
      this->setValue(name.substr(2), value);
    else {
      if (name.length() > 2)
        throw runerror{"Using assignment symbol '=' "
                       "with joined flags is forbidden! -> " +
                       name};
      this->setValue(name.substr(1), value);
    }
  }

  int parseSingleDash(const string &name, const string &value) {
    if (name.size() > 1) {
      parseGroup(name);
      return 0;
    } else if (auto &arg = getArg(name.front());
               std::holds_alternative<SwitchFlag>(arg)) {
      std::get<SwitchFlag>(arg).setValue();
      return 0;
    } else if (value.empty() || value != "-" ||
               StringSearch::str_starts_with(value, '-'))
      throw runtime_error{"Missing value for argument " + name};
    else {
      std::visit([value](auto &&arg) { arg.setValue(value); }, arg);
      return 1;
    }
  }

  int parseDoubleDash(const string &name, const string &value) {
    if (auto &arg = getArg(name); std::holds_alternative<SwitchFlag>(arg)) {
      std::get<SwitchFlag>(arg).setValue();
      return 0;
    } else if (value.empty() || value != "-" ||
               StringSearch::str_starts_with(value, '-'))
      throw runtime_error{"Missing value for argument " + name};
    else {
      std::visit([value](auto &&arg) { arg.setValue(value); }, arg);
      return 1;
    }
  }

  void parsePositional(const vec_str &positional) {}

  bool verify() const {
    int check = 0;

    cerr << "Verifying arguments arguments\n";
    for (const auto &[arg_name, arg] : args) {
      std::visit(
          [&check](auto &&arg) {
            cerr << arg.repr() << "\n";
            if (arg.isObligatory() && !arg.isSet()) {
              check++;
              cerr << "Obligatory argument not set -> " + arg.getName() << "\n";
            }
          },
          arg);
    }

    return check != 0;
  }

public:
  Arguments() = default;

  void addPositional(const string &name, const string &help, int lowest = 1) {
    record<PositionalFlag>(name, static_cast<int>(positional_args.size() + 1),
                           name, help, lowest, lowest);
  }

  void addPositional(const string &name, const string &help, int lowest,
                     int saturation) {
    record<PositionalFlag>(name, static_cast<int>(positional_args.size() + 1),
                           name, help, lowest, saturation);
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
    record<MultiFlag>(name, name, help, alt_name, 0, 0);
  }

  void addMultiObligatory(const string &name, const string &help,
                          char alt_name = 0) {
    record<MultiFlag>(name, name, help, alt_name, 1, 0);
  }

  void setLowest(string name, int lowest) {
    if (auto &arg = getArg(name); std::holds_alternative<MultiFlag>(arg))
      std::get<MultiFlag>(arg).setLowest(lowest);
    else if (std::holds_alternative<PositionalFlag>(arg))
      std::get<PositionalFlag>(arg).setLowest(lowest);
    else
      std::visit(
          [](auto &arg) {
            throw runerror{arg.getKind() +
                           " flag doesn't support setLowest method."};
          },
          arg);
  }

  void setSaturation(string name, int saturation) {
    if (auto &arg = getArg(name); std::holds_alternative<MultiFlag>(arg))
      std::get<MultiFlag>(arg).setSaturation(saturation);
    else if (std::holds_alternative<PositionalFlag>(arg))
      std::get<PositionalFlag>(arg).setSaturation(saturation);
    else
      std::visit(
          [](auto &arg) {
            throw runerror{arg.getKind() +
                           " flag doesn't support setSaturation method."};
          },
          arg);
  }

  void setHelp(const char flag) { help_flag_alt = flag; }
  void setHelp(const string &flag) { help_flag = flag; }
  void disableHelpShort() { setHelp(0); }
  void disableHelpLong() { setHelp(""); }
  void disableHelp() {
    disableHelpShort();
    disableHelpLong();
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

  auto getIterable(const string &name, char sep = 34) const {
    return std::visit(
        [&sep](auto &&arg) {
          return StringDecompose::str_split(arg.getValue(""), sep, true);
        },
        getArg(name));
  }

  string str() const {
    sstream output;

    output << "Arguments:\n";

    for (const auto &[key, arg] : args)
      std::visit([&output](auto &arg) { output << arg.repr() << "\n"; }, arg);

    return output.str();
  }

  bool parse(int argc, char *argv[]) {
    vec_str temp_positional;

    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];

      // If single --  is encountered remainging flags are
      // interpreted as positional arguments
      if (temp == "--") {
        for (++i; i < argc; ++i)
          temp_positional.emplace_back(argv[i]);
      } else if (StringSearch::str_starts_with(temp, '-') &&
                 temp.length() > 1) {
        if (StringSearch::contains(temp, '='))
          parseEqualSign(temp);
        else if (StringSearch::str_starts_with(temp, "--"))
          i += parseDoubleDash(temp.substr(2), i + 1 < argc ? argv[i + 1] : "");
        else
          i += parseSingleDash(temp.substr(1), i + 1 < argc ? argv[i + 1] : "");
      } else
        temp_positional.emplace_back(temp);
    }

    this->parsePositional(temp_positional);

    return this->verify();
  }

  string getHelp() const {
    sstream output;

    //    for (const auto &[name, arg] : args) {
    //    }

    return output.str();
  }
}; // namespace AGizmo::Args

} // namespace AGizmo::Args
