#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
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

struct FlagInfo {
  string name{};
  string help{};
  char name_alt{0};

  bool hasAltName() const noexcept { return name_alt; }

  string str() const {
    return name + (hasAltName() ? "/" + string(1, name_alt) : "");
  }
};

struct FlagValue {
  opt_str def_value{};
  opt_str value{};

  FlagValue() = default;
  FlagValue(const opt_str &default_value) : def_value{default_value} {}

  bool isSet() const noexcept { return this->value.has_value(); }
  opt_str getValue() const noexcept { return isSet() ? value : def_value; }
  string getValue(const string &backup) const noexcept {
    if (const auto result = getValue())
      return *result;
    else
      return backup;
  }
  void setValue(const opt_str &value) { this->value = value; }
  void reset() { setValue(nullopt); }

  void append(const string &value, char sep) {
    if (this->isSet())
      *this->value += string(1, sep) + value;
    else
      this->value = value;
  }

  FlagValue &operator=(const opt_str &value) {
    setValue(value);
    return *this;
  }

  FlagValue &operator=(const string &value) {
    setValue(value);
    return *this;
  }
};

class RegularFlag {
private:
  FlagInfo info;
  FlagValue value;
  inline const static string kind{"RegularFlag"};
  char sep{0};
  bool obligatory{false};

public:
  RegularFlag() = default;
  RegularFlag(const string &name, const string &help, char name_alt,
              const opt_str &default_value, bool obligatory, char sep = 0)
      : info{name, help, name_alt}, value{default_value}, sep{sep},
        obligatory{obligatory} {
    if (name.empty())
      throw runerror("Name of argument cannot be empty!");
    if (StringSearch::str_starts_with(name, "-"))
      throw runerror{"Name cannot not start with '-' sign"};
  }

  // Checkers

  bool isObligatory() const noexcept { return obligatory; }
  bool isSet() const noexcept { return this->value.isSet(); }
  bool isAppendable() const noexcept { return this->sep; }

  // Getters

  string getName() const noexcept { return this->info.name; }
  char getNameAlt() const noexcept { return this->info.name_alt; }
  string getHelp() const noexcept { return info.help; }
  string getKind() const noexcept { return kind; }

  opt_str getValue() const noexcept { return value.getValue(); }
  string getValue(const string &backup) const noexcept {
    return value.getValue(backup);
  }

  vec_str getIterable() const {
    return StringDecompose::str_split(this->value.getValue(""), sep, true);
  }

  // Setters

  void makeObligatory() { this->obligatory = true; }
  void setSeparator(char sep = 0) { this->sep = sep; }
  void enableAppend(char sep) { this->setSeparator(sep); }
  void disableAppend() { this->setSeparator(0); }

  void setValue(const opt_str &value) { this->value = value; }

  void append(const string &value, const char sep) {
    this->value.append(value, sep);
  }

  void setValue(const string &value) {
    if (this->isAppendable())
      this->append(value, sep);
    else
      this->setValue(value);
  }

  void reset() { value.reset(); }

  string repr() const {
    sstream output;
    output << "RegularFlag: " << info.str()
           << " Value: " << std::quoted(getValue("__Empty__"));
    return output.str();
  }

  //  string str_help() const { return str(); }
};

class MultiFlag {
private:
  inline static const string kind{"Multi"};
  FlagInfo info;
  vec_str value;
  int lowest{0};
  int saturation{0};

public:
  MultiFlag() = default;
  MultiFlag(const string &name, const string &help, char name_alt, int lowest,
            int saturation)
      : info{name, help, name_alt} {
    this->setLowest(lowest);
    this->setSaturation(saturation);
  }

  //
  [[nodiscard]] bool isSet() const noexcept { return !value.empty(); }
  [[nodiscard]] bool isObligatory() const noexcept { return lowest; }
  [[nodiscard]] bool isLoadable() const noexcept {
    return saturation < 1 || getValueCount() < saturation + 1;
  }

  // Getters

  string getName() const { return info.name; }
  char getNameAlt() const { return info.name_alt; }
  string getHelp() const { return info.help; }
  string getKind() const { return this->kind; }

  opt_str getValue() const {
    if (this->isSet())
      return StringCompose::str_join(this->value, 34);
    else
      return {};
  }

  string getValue(const string &backup) const {
    return isSet() ? StringCompose::str_join(this->value, 34) : backup;
  }

  vec_str getIterable() const { return this->value; }
  //  string getValue(const string &backup) const { return
  //  flag.getValue(backup); }
  int getValueCount() const { return static_cast<int>(value.size()); }
  int getSaturation() const { return this->saturation; }
  int getLowest() const { return this->lowest; }

  // Setters

  void setSaturation(int saturation) {
    if (saturation < 0)
      throw runerror{"Saturation value must be at least 0 or greater."};
    else if (saturation && saturation < this->lowest)
      throw runerror{"Saturation " + std::to_string(saturation) +
                     " is lower than the lowest posiible value " +
                     std::to_string(lowest)};
    else
      this->saturation = saturation;
  }

  void setLowest(int lowest) {
    if (lowest < 0)
      throw runerror{"Lowest value must be at least 0 or greater."};
    else if (this->saturation && lowest > this->saturation)
      throw runerror{"Lowest " + std::to_string(lowest) +
                     " value is higher than saturation " +
                     std::to_string(this->saturation)};
    else
      this->lowest = lowest;
  }

  //  opt_str setValue(const string &value) {
  void setValue(const string &value) { this->value.emplace_back(value); }

  template <class It> void setValue(It &begin, const It end) {
    const auto args_count = std::distance(begin, end);

    if (this->getLowest() > args_count)
      throw runerror{"Not enough arguments for PositionalFlag " +
                     this->getName()};
    else {
      //      const auto args_max
      while (begin < end) {
        if (this->isLoadable())
          this->setValue(*(begin++));
      }
    }
  }

  string repr() const {
    sstream output;
    output << "MultiFlag: " << info.str()
           << " Value: " << std::quoted(getValue("__Empty__"));
    return output.str();
  }
};

class PositionalFlag {
private:
  inline const static string kind{"Positional"};
  int position{1};
  MultiFlag flag{};

public:
  PositionalFlag() = default;
  PositionalFlag(int position, const string &name, const string &help,
                 int lowest, int saturation)
      : position{position}, flag{name, help, 0, lowest, saturation} {}

  // Checkers

  bool isSet() const { return flag.isSet(); }
  bool isObligatory() const { return flag.isObligatory(); }
  bool isLoadable() const { return this->flag.isLoadable(); }

  // Getters

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }
  string getHelp() const { return flag.getHelp(); }
  string getKind() const { return this->kind; }

  opt_str getValue() const { return flag.getValue(); }
  string getValue(const string &backup) const { return flag.getValue(backup); }
  vec_str getIterable() const { return flag.getIterable(); }
  int getValueCount() const { return flag.getValueCount(); }
  int getLowest() const { return flag.getLowest(); }
  int getSaturation() const { return flag.getSaturation(); }

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

  template <class It> void setValue(It &begin, const It end) {
    this->flag.setValue(begin, end);
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
  inline const static string kind = "Switch";
  FlagInfo info;
  bool value;

public:
  SwitchFlag() = default;
  SwitchFlag(const string &name, const string &help, char name_alt)
      : info{name, help, name_alt} {}

  bool isSet() const { return value; }
  bool isObligatory() const { return false; }

  void setValue(const bool value = true) { this->value = value; }
  void setValue(const opt_str &value) { setValue(value.has_value()); }
  void reset() { value = false; }

  // Getters

  string getName() const { return info.name; }
  char getNameAlt() const { return info.name_alt; }
  string getHelp() const { return info.help; }
  string getKind() const { return this->kind; }
  opt_str getValue() const {
    if (value)
      return "";
    else
      return nullopt;
  }
  string getValue(const string &backup) const {
    if (value)
      return "";
    else
      return backup;
  }

  vec_str getIterable() const { return isSet() ? vec_str{} : vec_str{""}; }

  string repr() const {
    sstream output;
    output << "Switch: " << info.str() << " Value: "
           << std::quoted(this->isSet() ? "__Set__" : "__NotSet__");
    return output.str();
  }
};

using Flags = std::variant<RegularFlag, PositionalFlag, MultiFlag, SwitchFlag>;
// using Flags = std::variant<PositionalFlag>;
using FlagsMap = std::unordered_map<string, Flags>;
using FlagsNamesAlt = std::unordered_map<char, string>;

using FlagOpt = std::optional<std::reference_wrapper<Flags>>;

class Arguments {
private:
  // Options

  char help_flag_alt{'h'};
  string help_flag{"help"};
  bool invoke_help{false};

  // Arguments
  FlagsMap args{};
  opt_int numerical_arg;
  vec_str positional_args;

  // Maps
  FlagsNamesAlt alt_names_map;
  vec_str positional_map;

  void insertPositional(const string &name) {
    positional_map.emplace_back(name);
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
    }
  }

  bool contains(const string &name) const {
    return args.find(name) != args.end();
  }

  opt_str contains(const char name) const {
    if (auto it = alt_names_map.find(name); it != alt_names_map.end())
      return it->second;
    else
      return nullopt;
  }

  auto &getArg(const string &name) {
    if (contains(name))
      return args[name];
    else
      //      return nullopt;
      throw runerror{"Argument " + name + " does not exist!"};
  }

  auto &getArg(const char name) {
    if (const auto arg_name = contains(name))
      return args[*arg_name];
    else
      //      return nullopt;
      throw runerror{"Symbol '" + string(1, name) + "' is not a valid flag!"};
  }

  auto &operator[](const string &name) { return args[name]; }

  bool matchesHelp(const string &flag) { return flag == help_flag; }
  bool matchesHelp(const char flag) { return flag == help_flag_alt; }

  void setValue(const string &name, const string &value = "") {
    std::visit([value](auto &&arg) { arg.setValue(value); }, getArg(name));
  }

  void setValue(const char name, const string &value = "") {
    std::visit([value](auto &&arg) { arg.setValue(value); }, getArg(name));
  }

  void setValue(size_t position, const string &value = "") {
    if (position < positional_map.size())
      setValue(positional_map[position], value);
    else
      throw runerror{"Argument with postion '" + to_string(position) +
                     "' was not added!"};
  }

  void parseGroup(const string &flag) {
    if (const auto number = StringFormat::str_to_int(flag))
      numerical_arg = *number;
    else {
      for (size_t flag_pos = 0; flag_pos < flag.size(); ++flag_pos) {
        if (const auto flag_symbol = flag[flag_pos]; matchesHelp(flag_symbol)) {
          invoke_help = true;
          return;
        } else if (const auto flag_name = contains(flag_symbol)) {
          if (auto &arg = args[*flag_name];
              std::holds_alternative<SwitchFlag>(arg))
            std::get<SwitchFlag>(arg).setValue();
          else if (const auto value = flag.substr(flag_pos + 1); value.empty())
            throw runerror{"Missing value for " + *flag_name + " !"};
          else
            std::visit([&value](auto &&arg) { arg.setValue(value); }, arg);
        } else
          throw runerror{"Could not recognize one of these flags: " + flag};
      }
    }
  }

  void parseEqualSign(const string &arg) {

    auto [name, value] = StringDecompose::str_split_in_half(arg, '=');
    cerr << name << " " << value << "\n";
    if (StringSearch::str_starts_with(name, "--"))
      this->setValue(name.substr(2), value);
    else {
      if (name.length() > 2)
        throw runerror{"Using assignment symbol '=' "
                       "with joined flags is forbidden! -> " +
                       arg};
      this->setValue(name.substr(1).front(), value);
    }
  }

  int parseSingleDash(const string &name, const string &value) {
    //    cerr << name << " " << value << "\n";
    if (name.size() > 1) {
      parseGroup(name);
      return 0;
    } else {
      if (matchesHelp(name.front())) {
        invoke_help = true;
        return 0;
      } else
        return std::visit(
            [value](auto &&arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, SwitchFlag>) {
                arg.setValue();
                return 0;
              } else if (value.empty() ||
                         (value != "-" &&
                          StringSearch::str_starts_with(value, '-')))
                throw runtime_error{"Missing value for argument " +
                                    arg.getName()};
              else {
                arg.setValue(value);
                return 1;
              }
            },
            getArg(name.front()));
    }
  }

  int parseDoubleDash(const string &name, const string &value) {
    if (matchesHelp(name)) {
      invoke_help = true;
      return 0;
    }

    return std::visit(
        [value](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, SwitchFlag>) {
            arg.setValue();
            return 0;
          } else if (value.empty() ||
                     (value != "-" &&
                      StringSearch::str_starts_with(value, '-')))
            throw runtime_error{"Missing value for argument " + arg.getName()};
          else {
            arg.setValue(value);
            return 1;
          }
        },
        getArg(name));
  }

  void parsePositional() {
    if (positional_map.empty())
      return;

    auto pos_arg = this->positional_args.begin();
    const auto pos_arg_end = this->positional_args.end();

    for (const auto &name : positional_map)
      std::get<PositionalFlag>(getArg(name)).setValue(pos_arg, pos_arg_end);
  }

  bool verify() const {
    int check = 0;

    cerr << "Verifying arguments arguments\n";
    for (const auto &[arg_name, arg] : args) {
      std::visit(
          [&check](auto &&arg) {
            cerr << arg.getName() << "\n";
            cerr << arg.getValue("__") << "\n";
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

  void addPositional(const string &name, const string &help, int lowest,
                     int saturation) {
    record<PositionalFlag>(name, static_cast<int>(positional_map.size() + 1),
                           name, help, lowest, saturation);
    insertPositional(name);
  }

  void addPositional(const string &name, const string &help, int lowest = 1) {
    this->addPositional(name, help, lowest, lowest);
  }

  void addArgument(const string &name, const string &help, char alt_name,
                   optional<string> def_value = nullopt) {
    record<RegularFlag>(name, name, help, alt_name, def_value, false);
  }

  void addArgument(const string &name, const string &help,
                   optional<string> def_value = nullopt) {
    this->addArgument(name, help, 0, def_value);
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

  void setLowest(string name, int lowest) {
    std::visit(
        [lowest](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, MultiFlag> ||
                        std::is_same_v<T, PositionalFlag>)
            arg.setLowest(lowest);
          else
            throw runerror{arg.getName() + "[" + arg.getKind() +
                           "] flag doesn't support setLowest method."};
        },
        getArg(name));
  }

  void setSaturation(string name, int saturation) {
    std::visit(
        [saturation](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, MultiFlag> ||
                        std::is_same_v<T, PositionalFlag>)
            arg.setSaturation(saturation);
          else
            throw runerror{arg.getName() + "[" + arg.getKind() +
                           "] flag doesn't support setSaturation method."};
        },
        getArg(name));
  }

  void enableAppend(const string &name, const char sep) {
    std::visit(
        [sep](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, RegularFlag>)
            arg.enableAppend(sep);
          else
            throw runerror{"Cannot enable append for " + arg.repr()};
        },
        getArg(name));
  }

  void disableAppend(const string &name) {
    std::visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, RegularFlag>)
            arg.disableAppend();
          else
            throw runerror{"Cannot make " + arg.repr() + " appendable."};
        },
        getArg(name));
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
    if (contains(name))
      return args.at(name);
    else
      throw runerror{"Failed to recognise argument " + name};
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

  template <class T> struct always_false : std::false_type {};

  vec_str getIterable(const string &name) const {
    return std::visit([](auto &&arg) { return arg.getIterable(); },
                      getArg(name));
  }

  int getNumerical() const noexcept { return *this->numerical_arg; }

  const vec_str &getPositional() const noexcept {
    return this->positional_args;
  }

  string str() const {
    sstream output;

    output << "Arguments:\n";

    for (const auto &[key, arg] : args)
      std::visit([&output](auto &arg) { output << arg.repr() << "\n"; }, arg);

    return output.str();
  }

  bool parse(int argc, char *argv[]) {
    positional_args.clear();

    for (int i = 1; i < argc; ++i) {
      string temp = argv[i];

      // If single --  is encountered remainging flags are
      // interpreted as positional arguments
      if (invoke_help)
        break;
      if (temp == "--") {
        for (++i; i < argc; ++i)
          positional_args.emplace_back(argv[i]);
      } else if (StringSearch::str_starts_with(temp, '-') &&
                 temp.length() > 1) {
        if (StringSearch::contains(temp, '='))
          parseEqualSign(temp);
        else if (StringSearch::str_starts_with(temp, "--"))
          i += parseDoubleDash(temp.substr(2), i + 1 < argc ? argv[i + 1] : "");
        else
          i += parseSingleDash(temp.substr(1), i + 1 < argc ? argv[i + 1] : "");
      } else
        positional_args.emplace_back(temp);
    }

    if (invoke_help) {
      cerr << getHelp() << "\n";
      return 1;
    } else {
      this->parsePositional();
      return this->verify();
    }
  }

  string getHelp() const {
    sstream output;

    //    for (const auto &[name, arg] : args) {
    //    }

    output << "Help!\n";

    return output.str();
  }
}; // namespace AGizmo::Args

} // namespace AGizmo::Args
