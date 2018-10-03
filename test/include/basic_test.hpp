#pragma once

#include "evaluation.h"
#include "strings.h"

#include <sstream>

using sstream = std::stringstream;

using namespace AGizmo;
using namespace Evaluation;
using namespace StringFormat;

template <typename It>
Stats test_XOR(It begin, It end, sstream &message);

template<typename Type>
struct PrintableVector {
  vector<Type> value{};

  PrintableVector(std::initializer_list<Type> input) : value{input} {}
  template< class InputIt >
  PrintableVector(InputIt first, InputIt last){
    value = vector<Type>(first, last);
  }

  string str() const{
    if (value.empty())
      return "{}";

    return "{" + str_join(value.begin(), value.end(), ", ") + "}, ";
  }

  auto begin() const noexcept {return value.begin(); }
  auto end() const noexcept {return value.end(); }

  bool operator==(const PrintableVector<Type> &other){
    return value == other.value;
  }
};

template<typename Type>
struct NestedVector{
  vector<PrintableVector<Type>> values{};

  NestedVector(std::initializer_list<PrintableVector<Type>> input):
    values{input} {}

  string str() const{
    if (values.empty())
      return "{{}}";
    else {
      sstream output;
      output << "{";

      for (const auto &ele: values) {
        output << "{" + str_join(ele.begin(), ele.end(), ", ") + "}, ";
      }

      return output.str() + "}";
    }
  }

  auto begin() const noexcept {return values.begin(); }
  auto end() const noexcept {return values.end(); }
  auto cend() const noexcept {return values.cend(); }

  bool operator==(const NestedVector<Type> &other){
    return values == other.values;
  }

  void push_back( const PrintableVector<Type>& value ) {
    values.push_back(value);
  }

};

//class BasicSplitTest {

//private:
//  using Input = vector<vector<int>>;
//  const vector<Input> input = {
//          {{}, {}},
//          {{1, 2, 3}, {}},
//          {{1, 2, 3}, {2}},
//          {{1, 2, 2, 3}, {2}},
//  };
//public:
//  SplitOutput() = default;
//  SplitOutput(vector<vector<int>> output): Output{move(output)} {
//  }
//  string str() const override {
//    if (output.empty())
//      return "{{}}";
//    else {
//      string result{"{"};

//      for (const auto &ele: output) {
//        vec_str temp{};

//        transform(ele.begin(), ele.end(), back_inserter(temp),
//                  [](int a){ return to_string(a);});

//        result += "{" + str_join(temp.begin(), temp.end(), ", ") + "}, ";
//      }
//      return result + "}";
//    }
//  }
//};
