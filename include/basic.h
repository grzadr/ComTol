#pragma once

#include <vector>
#include <stdexcept>

using std::pair;
using std::vector;
using std::runtime_error;

namespace AGizmo {

namespace Basic {

// Exclusive OR with two template arguments of the same type.
// Works with object having "operator!".
template <typename T>
bool constexpr XOR(const T &first, const T &second) noexcept {
  return first ? !second : second;
}

// Split query sequence(string, vector, etc.) using separator sep and
// inserting element into output using iterators.
// Function requires providing output type and search algorithm.
// Function returns output iterator, like transform function.

template <typename OutType, typename InIt, typename OutIt, typename Sep>
inline OutIt constexpr split(InIt query, const InIt &query_end, Sep sep,
                             OutIt output) {
  auto found(find(query, query_end, sep));

  do {
    *output++ = OutType(query, found);
    query = next(found);
    found = find(query, query_end, sep);
  } while (prev(query) != query_end);

  return output;
}

template <typename OutType, template <class> class Comp, typename InIt,
          typename OutIt, typename SepIt>
inline OutIt constexpr split(InIt query, const InIt &query_end, SepIt sep,
                             SepIt sep_end, OutIt output) {
  auto dist = distance(sep, sep_end);

  if (!dist) {
    *output++ = OutType(query, query_end);
    return output;
  }

  if (dist == 1)
    return split<OutType>(query, query_end, *sep, output);

  auto found(search(query, query_end, Comp<SepIt>(sep, sep_end)));

  do {
    *output++ = OutType(query, found);
    query = next(found, dist);
    found = (search(query, query_end, Comp<SepIt>(sep, sep_end)));
  } while (prev(query, dist) != query_end);

  return output;
}

// Function creates vector of pairs of two input types from iterators.
// Does not check if vectors are the same length
template <typename First, typename Second, typename FirstIt, typename SecondIt>
vector<pair<First, Second>> constexpr pairify(FirstIt fbegin, FirstIt fend,
                                              SecondIt sbegin, SecondIt send) {
  vector<pair<First, Second>> result{};
  result.reserve(fend - fbegin);
  for (; fbegin != fend && sbegin != send; ++fbegin, ++sbegin)
    result.push_back({*fbegin, *sbegin});
  return result;
}

// Wrap function that accepts const references to
// two vector that will be pairified.
// Check if vector has elements and both are the same size.
template <typename First, typename Second>
vector<pair<First, Second>> constexpr pairify(const vector<First> first,
                                              const vector<Second> second) {
  if (!first.size() || first.size() != second.size())
    throw runtime_error("Vectors have invalid sizes.");
  return pairify<First, Second>(first.begin(), first.end(), second.begin(),
                                second.end());
}

template <typename KeytIt, typename ValueIt, typename OutIt>
OutIt constexpr mapify(KeytIt kbegin, KeytIt kend, ValueIt vbegin, ValueIt vend,
                       OutIt output) {
  for (auto key = kbegin, value = vbegin; key != kend && value != vend;)
    *output++ = {*key++, *value++};
  return output;
}

} // namespace Basic
} // namespace AGizmos
