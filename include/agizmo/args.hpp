#pragma once

#include <algorithm>
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

class BaseFlag {
private:
  string name;
  string help;
  char name_alt;
  optional<string> value;
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

  // Getters

  string getName() const { return name; }
  char getNameAlt() const { return name_alt; }

  opt_str getValue() const { return value; }
  string getValue(const string &backup) const { return value.value_or(backup); }

  string getHelp() const { return help; }

  // Setters

  opt_str setValue(const opt_str &value = nullopt) {
    if (value.has_value() && StringSearch::str_starts_with(*value, "-"))
      throw runerror{"Value '" + *value + "' is not vali for argument '" +
                     name + "'"};
    auto temp = getValue();
    this->value = value;
    return temp;
  }

  opt_str reset() { return setValue(); }

  string str() const {
    sstream output;
    output << getName();

    if (name_alt)
      output << "/" << name_alt;

    output << getValue("__NULL__");

    return output.str();
  }

  string str_help() const { return str(); }
};

class PositionalFlag {
private:
  BaseFlag flag;
  int position;

public:
  PositionalFlag() = delete;
  PositionalFlag(int position, const string &name, const string &help,
                 bool obligatory)
      : flag{name, help, 0, nullopt, obligatory} {
    if (position < 1)
      throw runerror{"Position must be an positive integer!"};
    else
      this->position = position;
  }

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }

  string str() const {
    sstream output;
    output << "@" << position << " " << flag.str();
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

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }

  string str() const { return flag.str(); }
};

class SwitchFlag {
private:
  BaseFlag flag;

public:
  SwitchFlag() = delete;
  SwitchFlag(const string &name, const string &help, char name_alt)
      : flag{name, help, name_alt, nullopt, false} {}

  string getName() const { return flag.getName(); }
  char getNameAlt() const { return flag.getNameAlt(); }

  string str() const { return flag.str(); }
};

// using Flags = std::variant<BaseFlag, PositionalFlag, MultiFlag, SwitchFlag>;
using Flags = std::variant<PositionalFlag>;
using FlagsMap = std::unordered_map<string, Flags>;
using FlagsNamesAlt = std::unordered_map<char, string>;

class NewArguments {
private:
  // Options
  bool allow_overwrite{true};

  // Arguments
  FlagsMap args{};
  //  vector<Flags> args{};

  // Maps
  FlagsNamesAlt alt_names_map;
  vec_str positional;

  //  void record(const Flags &item) {}

  void record(int position, const string &name, const string &help,
              bool obligatory) {

    if (const auto &[it, inserted] =
            args.try_emplace(name, std::in_place_type<PositionalFlag>, position,
                             name, help, obligatory);
        !inserted)
      throw runerror{"Argument " + name + " alredy exists!"};
    else {
      std::visit(
          [this, &name](const auto &arg) {
            if (const auto name_alt = arg.getNameAlt(); name_alt)
              if (const auto &[it, inserted] =
                      this->alt_names_map.try_emplace(name_alt, name);
                  !inserted)
                throw runerror{"Argument " + name + " alredy exists!"};
          },
          it->second);
    }
    //    std::cerr << it->first << "\n";
    //    if (const auto &[it, inserted] =
    //            args.try_emplace(name, std::in_place_type<PositionalFlag>,
    //            position,
    //                             name, help, obligatory);
    //        !inserted)
    //
    //    else {
    //      std::visit(
    //          [this, &name](const auto &arg) {
    //            if (const auto name_alt = arg.getNameAlt(); name_alt)
    //              if (const auto &[it, inserted] =
    //                      this->alt_names_map.try_emplace(name_alt, name);
    //                  !inserted)
    //                throw runerror{"Argument " + name + " alredy exists!"};
    //          },
    //          it->second);
    //    }
  }

  template <class T, class... Args>
  void record(const string &name, Args &&... arguments) {
    if (const auto &[it, inserted] =
            args.try_emplace(name, std::in_place_type<T>, arguments...);
        !inserted)
      throw runerror{"Argument " + name + " alredy exists!"};
    else {
      std::visit(
          [this, &name](const auto &arg) {
            if (const auto name_alt = arg.getNameAlt(); name_alt)
              if (const auto &[it, inserted] =
                      this->alt_names_map.try_emplace(name_alt, name);
                  !inserted)
                throw runerror{"Argument " + name + " alredy exists!"};
          },
          it->second);
    }
  }

public:
  NewArguments() = default;

  void addPositional(const string &name, const string &help,
                     bool obligatory = false) {
    //    if (const auto &[it, inserted] =
    //            args.try_emplace(name, static_cast<int>(positional.size() +
    //            1),
    //                             name, help, obligatory);
    //        !inserted)
    //      throw runerror{"Argument " + name + " alredy exists!"};
    //    else
    //      record(it->second);

    record<PositionalFlag>(name, static_cast<int>(positional.size() + 1), name,
                           help, obligatory);
  }

  //  void addArgument(const string &name, const string &help, char alt_name,
  //                   optional<string> def_value = nullopt) {
  //    //    if (const auto &[it, inserted] =
  //    //            args.try_emplace(name, name, help, alt_name, def_value,
  //    //            false);
  //    //        !inserted)
  //    //      throw runerror{"Argument " + name + " alredy exists!"};
  //    //    else
  //    //      record(it->second);
  //    record<BaseFlag>(name, name, help, alt_name, def_value, false);
  //  }

  //  void addArgument(const string &name, const string &help,
  //                   optional<string> def_value = nullopt) {
  //    record<BaseFlag>(name, name, help, 0, def_value);
  //  }

  //  void addObligatory(const string &name, const string &help,
  //                     char alt_name = 0) {
  //    record<BaseFlag>(name, name, help, alt_name, nullopt, true);
  //  }

  //  void addSwitch(const string &name, const string &help, char alt_name = 0)
  //  {
  //    record<SwitchFlag>(name, name, help, alt_name);
  //  }

  //  void addMulti(const string &name, const string &help, char alt_name = 0) {
  //    record<MultiFlag>(name, name, help, alt_name, false);
  //  }

  //  void addMultiObligatory(const string &name, const string &help,
  //                          char alt_name = 0) {
  //    record<MultiFlag>(name, name, help, alt_name, true);
  //  }

  string str() const {
    sstream output;

    output << "Arguemnts:\n";

    for (const auto &[key, arg] : args)
      std::visit([&output](auto &arg) { output << arg.str() << "\n"; }, arg);

    return output.str();
  }

  bool parse(int argc, char *argv[]) {
    std::cerr << str() << "\n";
    return false;
  }
};

// class Flag {
// private:
//  int position;
//  string name;
//  string help;
//  ValueType value_type;
//  int value_count;
//  char alt_name;
//  optional<string> value;
//  bool obligatory;

// public:
//  Flag() = delete;
//  Flag(int position, const string &name, const string &help,
//       ValueType value_type, char alt_name, optional<string> default_val,
//       bool obligatory)
//      : position{position}, name{name}, help{help}, value_type{value_type},
//        alt_name{alt_name}, value{default_val}, obligatory{obligatory} {}

//  friend bool operator<(const Flag &lhs, const Flag &rhs) {
//    if (lhs.position == 0)
//      return false;
//    else if (rhs.position == 0)
//      return true;
//    else
//      return lhs.position < rhs.position;
//  }

//  // Identity check

//  bool isSet() const { return value.has_value(); }
//  bool isPositional() const { return position > 0; }
//  bool isSwitch() const { return value_type == ValueType::Switch; }
//  bool isMultiple() const { return value_type == ValueType::Multiple; }
//  bool isSingle() const { return value_type == ValueType::Single; }
//  bool isObligatory() const { return obligatory; }

//  // Modifiers
//  void makeObligatory() { obligatory = true; }
//  void makeOptional() { obligatory = false; }
//  auto makeMultiple() { this->value_type = ValueType::Multiple; }
//  auto makeSingle() { this->value_type = ValueType::Single; }

//  // Getters
//  auto getName() const noexcept { return name; }
//  auto getAltName() const noexcept { return alt_name; }
//  auto getPosition() const { return position; }
//  auto getValueType() const { return value_type; }

//  auto getValue() const {
//    if (this->isSet())
//      return *value;
//    else
//      throw runerror{"Argument " + name + " is not set!"};
//  }
//  auto getValue(const string &backup) const { return value.value_or(backup); }

//  // Setters
//  void reset() { value = nullopt; }
//  opt_str setValue(const string &value = "") {
//    if (StringSearch::str_starts_with(value, "-"))
//      throw runerror{"Missing value for " + name + "!"};
//    else if (value_type == ValueType::Multiple) {
//      //      this->value =
//      //          (this->isSet() +) this->value.value_or("") + string(1, 34) +
//      //          value;
//      return nullopt;
//    } else {
//      bool result = this->value.has_value();
//      this->value = value;
//      //      return result;
//    }
//  }

//  string str() const {
//    sstream output;
//    if (isPositional())
//      output << "@" << to_string(getPosition()) << " ";
//    else
//      output << "--";
//    output << getName();

//    if (alt_name)
//      output << "/" << alt_name;

//    output << ":" << str_value_type(getValueType()) << " -> ";
//    if (isSwitch())
//      output << (isSet() ? "On" : "Off");
//    else
//      output << getValue("None");

//    return output.str();
//  }

//  friend std::ostream &operator<<(std::ostream &stream, const Flag &item) {
//    return stream << item.str();
//  }
//};

// class Arguments {
// private:
//  // Options
//  bool allow_overwrite{true};
//  vector<Flag> args{};

//  // Maps

//  args_map alt_names_map;
//  vec_str positional;

//  void addArgument(int position, const string &name, const string &help,
//                   const ValueType &value_type, char alt_name,
//                   const optional<string> &def_value, bool obligatory) {
//    if (name.empty())
//      throw runtime_error{"Argument name cannot be empty!"};
//    if (contains(name))
//      throw runerror("Argument '" + name + "' already added!");

//    if (auto [it, inserted] = alt_names_map.try_emplace(alt_name, name);
//        !inserted)
//      throw runerror("Argument '" + string(1, alt_name) + "' already added!");

//    args.emplace_back(position, name, help, value_type, alt_name, def_value,
//                      obligatory);
//  }

//  auto &getArg(const string &name) {
//    for (auto &ele : args) {
//      if (ele.getName() == name)
//        return ele;
//    }

//    throw runtime_error("Argument not found:" + name);
//  }

//  auto getArg(const string &name) const {
//    for (auto &ele : args) {
//      if (ele.getName() == name)
//        return ele;
//    }

//    throw runtime_error("Argument not found:" + name);
//  }

//  auto &getArg(const int position) {
//    for (auto &ele : args) {
//      if (position == ele.getPosition())
//        return ele;
//    }

//    throw runtime_error("Positional argument not found:" +
//    to_string(position));
//  }

//  string getValue(int position) { return getArg(position).getValue(); }
//  string getValue(int position, const string &default_value) {
//    return getArg(position).getValue(default_value);
//  }

// public:
//  Arguments() = default;
//  Arguments(bool allow_overwrite) : allow_overwrite{allow_overwrite} {}

//  void addPositional(const string &name, const string &help,
//                     bool obligatory = false) {
//    addArgument(++last_position, name, help, ValueType::Single, 0, nullopt,
//                obligatory);
//  }

//  void addArgument(const string &name, const string &help, char alt_name,
//                   optional<string> def_value = nullopt) {
//    addArgument(0, name, help, ValueType::Single, alt_name, def_value, false);
//  }

//  void addArgument(const string &name, const string &help,
//                   optional<string> def_value = nullopt) {
//    addArgument(0, name, help, ValueType::Single, 0, def_value, false);
//  }

//  void addObligatory(const string &name, const string &help,
//                     char alt_name = 0) {
//    addArgument(0, name, help, ValueType::Single, alt_name, nullopt, true);
//  }

//  void addSwitch(const string &name, const string &help, char alt_name = 0) {
//    addArgument(0, name, help, ValueType::Switch, alt_name, nullopt, false);
//  }

//  auto begin() const { return this->args.begin(); }
//  auto cbegin() const { return this->args.cbegin(); }
//  auto end() const { return this->args.end(); }
//  auto cend() const { return this->args.cend(); }

//  auto size() const { args.size(); }
//  auto empty() const { args.empty(); }

//  void makeObligatory(const string &name) { getArg(name).makeObligatory(); }
//  void allowMultipleValues(const string &name) { getArg(name).makeMultiple();
//  }

//  bool contains(const string &name) const {
//    return std::find_if(begin(), end(), [&name](auto &a) {
//             return a.getName() == name;
//           }) != end();
//  }

//  bool isSet(const string &name) const { return getArg(name).isSet(); }

//  string getValue(const string &name) const { return getArg(name).getValue();
//  } string getValue(const string &name, const string &default_value) const {
//    return getArg(name).getValue(default_value);
//  }

//  string operator()(const string &name) const { return getValue(name); }
//  string operator()(const string &name, const string &default_value) const {
//    return getValue(name, default_value);
//  }

//  bool setValue(const string &name, const string &value = "") {
//    if (auto overwrite = getArg(name).setValue(value);
//        overwrite && !allow_overwrite)
//      throw runerror("Argument " + name + "overwritten!");
//    else
//      return false;
//  }

//  bool setValue(int position, const string &value = "") {
//    if (auto overwrite = getArg(position).setValue(value);
//        overwrite && !allow_overwrite)
//      throw runerror("Argument " + name + "overwritten!");
//    else
//      return false;
//  }

//  void parse(int argc, char *argv[]) {
//    std::stable_sort(args.begin(), args.end());

//    int current_position = 0;
//    for (int i = 1; i < argc; ++i) {
//      string temp = argv[i];
//      std::cerr << temp << "\n";
//      // If single --  is encountered rest are
//      // interpreted as positional arguments
//      if (temp == "--") {
//        for (; i < argc; ++i)
//          setValue(++current_position, argv[i]);
//      }
//      //      TODO Should parser allow lonely '-' sign?
//      //      else if (temp == "-")
//      //        throw runtime_error{"Missing option after '-' in position " +
//      //                            to_string(i)};
//      // Argument starts with "--"
//      else if (StringSearch::str_starts_with(temp, "--")) {
//        if (auto arg_name = temp.substr(2);
//            !StringSearch::contains(arg_name, '=')) {

//          if (auto arg_ref = getArg(arg_name); arg_ref.isSwitch())
//            arg_ref.setValue();
//          else if (++i == argc)
//            throw runtime_error{"Missing value for argument " + temp};
//          else
//            arg_ref.setValue(argv[i]);

//        } else {
//          auto [name, value] =
//              StringDecompose::str_split_in_half(arg_name, '=');
//          setValue(name, value);
//        }

//      } else if (temp.front() == '-') {
//        if (temp = temp.substr(1); temp.size() == 1) {
//          if (const auto &arg_name = getArgName(temp.front())) {
//            auto &arg_ref = getArg(*arg_name);
//            if (arg_ref.isSwitch())
//              arg_ref.setValue("");
//            else if (++i == argc)
//              throw runtime_error{"Missing value for argument " + temp};
//            else
//              arg_ref.setValue(argv[i]);
//          } else {
//            throw runtime_error{"Unknown argument " +
//            to_string(temp.front())};
//          }
//        } else {
//          //        string last_name = "";
//          //        for (const auto &ele : temp) {
//          //            if (auto arg_name = get)
//          //        }
//          throw runtime_error{"Multiple argument with '-' not supported. "
//                              "Contact lazy developer."};
//        }
//      } else
//        this->getArg(++last_poz).setValue(argv[i]);
//    }

//    std::cerr << "Processed arguments\n";
//    for (const auto &arg : args) {
//      std::cerr << arg << "\n";
//      if (arg.isObligatory() && arg.isEmpty())
//        throw runtime_error{"Obligatory argument not set -> " + arg.str()};
//    }
//  }
//};

} // namespace AGizmo::Args
