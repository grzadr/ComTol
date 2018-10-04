#include "basic.h"
#include "strings.h"

#include "basic_test.hpp"

#include <iostream>
#include <memory>

#include <numeric>

using std::cout;
using std::endl;

using std::unique_ptr;

using namespace StringFormat;
// using Evaluation::Stats;
// using Evaluation::passed_str;
// using Evaluation::failed_str;

using StringFormat::str_frame;

using std::accumulate;
using std::move;
using std::next;
using std::to_string;

XORWithBool::XORWithBool(Input input, Output expected)
    : input{input}, expected{expected} {
  validate();
}

XORWithChar::XORWithChar(Input input, Output expected)
    : input{input}, expected{expected} {
  validate();
}

Stats check_XOR(bool verbose = false) {
  Stats result;
  sstream message;

  message << "~~~ Checking Basic::XOR\n";

  message << "\nBoolean Test:\n";

  vector<XORWithBool> set_xor_with_bool{
      {{false, false}, false},
      {{true, false}, true},
      {{false, true}, true},
      {{true, true}, false},
  };

  Evaluator xor_with_bool("Basic::XOR", set_xor_with_bool);
  result(xor_with_bool.verify(message));

  message << "\nChar Test:\n";

  vector<XORWithChar> set_xor_with_char{
      {{0, 0}, false},
      {{0, '+'}, true},
      {{'+', 0}, true},
      {{'+', '+'}, false},
  };

  Evaluator xor_with_char("Basic::XOR", set_xor_with_char);
  result(xor_with_char.verify(message));

  message << "\n";

  if (result.hasFailed() || verbose) cout << message.str();

  cout << "~~~ " << gen_summary(result, "Checking Basic::XOR") << endl;

  return result;
}

SplitVectorWithVectorSep::SplitVectorWithVectorSep(Input input, Sep sep,
                                                   Output expected)
    : input{input}, sep{sep}, expected{expected} {
  validate();
}

SplitVectorWithSep::SplitVectorWithSep(Input input, Sep sep, Output expected)
    : input{input}, sep{sep}, expected{expected} {
  validate();
}

Stats check_split(bool verbose = false) {
  Stats result;
  sstream message;

  message << "\n~~~ Checking Basic::split\n";

  message << "\nTesting vector<int> with int separator:\n";

  vector<SplitVectorWithSep> tests_int = {
      {{}, 2, {{}}},
      {{1, 2, 3}, 4, {{1, 2, 3}}},
      {{1, 1, 2, 3, 3}, 2, {{1, 1}, {3, 3}}},
      {{1, 1, 2}, 2, {{1, 1}, {}}},
      {{2, 3, 3}, 2, {{}, {3, 3}}},
      {{2}, 2, {{}, {}}},
      {{1, 1, 2, 3, 3, 2}, 2, {{1, 1}, {3, 3}, {}}},
      {{2, 1, 1, 2, 3, 3}, 2, {{}, {1, 1}, {3, 3}}},
      {{2, 1, 1, 2, 3, 3, 2}, 2, {{}, {1, 1}, {3, 3}, {}}},
      {{2, 2}, 2, {{}, {}, {}}},
  };

  Evaluator test_split_int("Basic::split", tests_int);
  result(test_split_int.verify(message));

  message << "\nTesting vector<int> with "
             "vector<int> separator:\n";

  vector<SplitVectorWithVectorSep> tests = {
      {{}, {}, {{}}},
      {{1, 2, 3}, {}, {{1, 2, 3}}},
      {{1, 2, 3}, {4}, {{1, 2, 3}}},
      {{1, 1, 2, 3, 3}, {2}, {{1, 1}, {3, 3}}},
      {{1, 1, 2}, {2}, {{1, 1}, {}}},
      {{2, 3, 3}, {2}, {{}, {3, 3}}},
      {{2}, {2}, {{}, {}}},
      {{1, 1, 2, 3, 3, 2}, {2}, {{1, 1}, {3, 3}, {}}},
      {{2, 1, 1, 2, 3, 3}, {2}, {{}, {1, 1}, {3, 3}}},
      {{2, 1, 1, 2, 3, 3, 2}, {2}, {{}, {1, 1}, {3, 3}, {}}},
      {{1, 1, 2, 2, 3, 3}, {2, 2}, {{1, 1}, {3, 3}}},
      {{2, 2, 1, 1, 2, 2, 3, 3, 2, 2}, {2, 2}, {{}, {1, 1}, {3, 3}, {}}},
      {{2, 2, 1, 1, 2, 2, 3, 3}, {2, 2}, {{}, {1, 1}, {3, 3}}},
      {{1, 1, 2, 2, 3, 3, 2, 2}, {2, 2}, {{1, 1}, {3, 3}, {}}},
      {{1, 1, 2, 2}, {2, 2}, {{1, 1}, {}}},
      {{2, 2, 3, 3}, {2, 2}, {{}, {3, 3}}},
      {{2, 2}, {2, 2}, {{}, {}}},
      {{2, 2}, {2}, {{}, {}, {}}},
  };

  Evaluator test_split("Basic::split", tests);
  result(test_split.verify(message));

  message << "\n";

  if (result.hasFailed() || verbose) cout << message.str();

  cout << "~~~ " << gen_summary(result, "Checking Basic::split function")
       << "\n"
       << endl;

  return result;
}

// pair_int check_str_join_fields(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking str_replace function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const umap_str input_str = {{"K3", "V3"}, {"K2", ""}, {"K1", "V1"}};
//   auto expected = "K1=V1;K2=;K3=V3";
//   auto result = str_join_fields(input_str);
//
//   message << std::setw(3) << std::right << ++total << ")";
//   if (result != expected) {
//     ++failed;
//     message << std::setw(20) << std::right << result << " != " <<
//     std::setw(20)
//             << std::left << expected << " " << failed_str << "\n";
//   } else
//     message << std::setw(20) << std::right << result << " == " <<
//     std::setw(20)
//             << std::left << expected << " " << passed_str << "\n";
//
//   const umap<string, opt_str> input_opt = {
//       {"K3", ""}, {"K2", {}}, {"K1", "V1"}};
//   expected = "K1=V1;K2;K3=";
//   result = str_join_fields(input_opt);
//
//   message << std::setw(3) << std::right << ++total << ")";
//   if (result != expected) {
//     ++failed;
//     message << std::setw(20) << std::right << result << " != " <<
//     std::setw(20)
//             << std::left << expected << " " << failed_str << "\n";
//   } else
//     message << std::setw(20) << std::right << result << " == " <<
//     std::setw(20)
//             << std::left << expected << " " << passed_str << "\n";
//
//   const vec_str names{"K3", "K2"};
//   expected = "K3=V3;K2=";
//   result = str_join_fields(input_str, names);
//
//   message << std::setw(3) << std::right << ++total << ")";
//   if (result != expected) {
//     ++failed;
//     message << std::setw(20) << std::right << result << " != " <<
//     std::setw(20)
//             << std::left << expected << " " << failed_str << "\n";
//   } else
//     message << std::setw(20) << std::right << result << " == " <<
//     std::setw(20)
//             << std::left << expected << " " << passed_str << "\n";
//
//   expected = "K3=;K2";
//   result = str_join_fields(input_opt, names);
//
//   message << std::setw(3) << std::right << ++total << ")";
//   if (result != expected) {
//     ++failed;
//     message << std::setw(20) << std::right << result << " != " <<
//     std::setw(20)
//             << std::left << expected << " " << failed_str << "\n";
//   } else
//     message << std::setw(20) << std::right << result << " == " <<
//     std::setw(20)
//             << std::left << expected << " " << passed_str << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_str_replace(bool verbose = false) {
//   struct Element {
//     string source, query, value, expected;
//   };
//
//   int total = 0, failed = 0;
//   cout << "~~~ Checking str_replace function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const vector<Element> input{
//       {"0034007800", "00", "___", "___34___78___"},
//       {"ABAABBABCAB", "", "_", "ABAABBABCAB"},
//       {"ABAABBABCAB", "AB", "", "ABC"},
//       {"ABAABBABCAB", "AB", "_", "_A_B_C_"},
//       {"ABAABBABCAB", "AB", "__", "__A__B__C__"},
//       {"ABAABBABCAB", "AB", "___", "___A___B___C___"},
//       {"ABC", "B", "", "AC"},
//       {"ABC", "B", "D", "ADC"},
//   };
//
//   for (const auto &[source, query, value, expected] : input) {
//     auto result = source;
//     str_replace(result, query, value);
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//             << std::left
//             << " str_replace(\"" + source + "\", \"" + query + "\", \"" +
//                    value + "\")"
//             << " -> " << std::setw(20) << std::right << "\"" + result + "\"
//             ";
//     if (result != expected) {
//       ++failed;
//       message << std::setw(20) << std::left << "!= \"" + expected + "\" "
//               << failed_str << "\n";
//     } else
//       message << std::setw(20) << std::left << "== \"" + expected + "\" "
//               << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   const map<string, string> input_map{
//       {"AB", "__"}, {"C", "++"}, {"DE", ""}, {"==", "##"}, {"++", "AB"}};
//   string source{"++DEXABDEYDEC"}, expected{"__X__Y++"};
//   auto result = source;
//   str_replace_n(result, input_map);
//
//   message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//           << std::left << " str_replace(\"" + source + "\", input_map)"
//           << " -> " << std::setw(20) << std::right << "\"" + result + "\" ";
//   if (result != expected) {
//     ++failed;
//     message << std::setw(20) << std::left << "!= \"" + expected + "\" "
//             << failed_str << "\n";
//   } else
//     message << std::setw(20) << std::left << "== \"" + expected + "\" "
//             << passed_str << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_str_map_fields(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking str_map_fields function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const vector<pair<string, umap<string, opt_str>>> input{
//       {"K1=V1;K2;K3=", {{"K1", "V1"}, {"K2", {}}, {"K3", ""}}},
//   };
//
//   for (const auto &[source, expected] : input) {
//     const auto result = str_map_fields(source);
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//             << std::left << " str_map_fields(\"" + source + "\", input_map)";
//     if (result != expected) {
//       ++failed;
//       message << failed_str << "\n";
//     } else
//       message << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_str_clean(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking pairify function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const vector<pair_str> input_simple = {
//       {"", ""},
//       {" \t  \t ", ""},
//       {" \t A \t \t B \t ", "A B"},
//       {"A \t \tB", "A B"},
//       {"\t A  B", "A B"},
//       {"A  B \t", "A B"},
//   };
//
//   for (const auto &[query, expected] : input_simple) {
//     const auto result = str_clean(query);
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//             << std::left << " str_clean(\"" + query + "\")"
//             << " -> \"" << result << "\" ";
//     if (result != expected) {
//       ++failed;
//       message << "!= \"" << expected << "\" " << failed_str << "\n";
//     } else
//       message << "== \"" << expected << "\" " << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   const vector<pair_str> input_strip = {
//       {"", ""},
//       {" \t  \t\t  \t ", "\t \t\t \t"},
//       {"A\t   \tB", "A\t \tB"},
//   };
//
//   for (const auto &[query, expected] : input_strip) {
//     const auto result = str_clean(query, true, " \n");
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//             << std::left << " str_clean(\"" + query + "\")"
//             << " -> \"" << result << "\" ";
//     if (result != expected) {
//       ++failed;
//       message << "!= \"" << expected << "\" " << failed_str << "\n";
//     } else
//       message << "== \"" << expected << "\" " << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_str_clean_ends(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking clean_ends function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const vector<pair_str> input_simple = {
//       {" \t A  B \t ", "A  B"}, {" \t  \t ", ""},     {"", ""},
//       {"A  B", "A  B"},         {" \t A  B", "A  B"}, {"A  B \t ", "A  B"},
//   };
//
//   for (const auto &[query, expected] : input_simple) {
//     const auto result = str_clean_ends(query);
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//             << std::left << " str_clean_ends(\"" + query + "\")"
//             << " -> \"" << result << "\" ";
//     if (result != expected) {
//       ++failed;
//       message << "!= \"" << expected << "\" " << failed_str << "\n";
//     } else
//       message << "== \"" << expected << "\" " << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   const vector<pair_str> input_strip = {
//       {"\t A  B \t", " A  B "}, {"\t  \t", "  "},     {"", ""},
//       {"A  B", "A  B"},         {"\t A  B", " A  B"}, {"A  B \t", "A  B "},
//   };
//
//   for (const auto &[query, expected] : input_strip) {
//     const auto result = str_clean_ends(query, "\t\n");
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(40)
//             << std::left << " str_clean_ends(\"" + query + "\")"
//             << " -> \"" << result << "\" ";
//     if (result != expected) {
//       ++failed;
//       message << "!= \"" << expected << "\" " << failed_str << "\n";
//     } else
//       message << "== \"" << expected << "\" " << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_str_split(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking pairify function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const vector<tuple<string, char, vector<string>>> input_char{
//       {"", 0, {}},
//       {"ABC", 0, {"ABC"}},
//       {"", ',', {}},
//       {",", ',', {"", ""}},
//       {",ABC,", ',', {"", "ABC", ""}},
//   };
//
//   for (const auto &[query, sep, expected] : input_char) {
//     const auto result = str_split(query, sep, true);
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(25)
//             << std::left
//             << " str_split(\"" + query + "\", '" + string(1, sep) + "')"
//             << " -> " << result.size() << " ";
//     if (result != expected) {
//       ++failed;
//       message << "!= " << expected.size() << " " << failed_str << "\n";
//     } else
//       message << "== " << expected.size() << " " << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_only_digits(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking pairify function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   vector<pair<string, bool>> input{{"0", true},
//                                    {"-1", false},
//                                    {"", false},
//                                    {"3.14", false},
//                                    {"10E2", false}};
//
//   for (const auto &[query, expected] : input) {
//     const auto result = only_digits(query);
//     message << std::setw(3) << std::right << ++total << ")" << std::setw(20)
//             << std::right << " only_digits(\"" + query + "\")"
//             << " -> " << result << " ";
//     if (result != expected) {
//       ++failed;
//       message << "!= " << expected << " " << failed_str << "\n";
//     } else
//       message << "== " << expected << " " << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_pairify(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking pairify function" << endl;
//
//   sstream message;
//
//   message << "\n";
//
//   const vector<tuple<vector<string>, vector<int>, vector<pair<string, int>>>>
//       input{
//           {{"A", "B", "C"}, {1, 2, 3}, {{"A", 1}, {"B", 2}, {"C", 3}}},
//       };
//
//   for (const auto &[first, second, expected] : input) {
//     message << std::setw(3) << std::right << ++total << ") ";
//
//     if (pairify(first, second) != expected) {
//       ++failed;
//       message << failed_str << "\n";
//     } else
//       message << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int check_split(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking split function" << endl;
//
//   sstream message;
//
//   const vector<tuple<string, string, vector<string>>> input_string{
//       {"", "", {""}},
//       {"", "_", {""}},
//       {"ABC_DEF", "", {"ABC_DEF"}},
//       {"ABC_DEF", "_", {"ABC", "DEF"}},
//       {"_ABC_DEF", "_", {"", "ABC", "DEF"}},
//       {"ABC_DEF_", "_", {"ABC", "DEF", ""}},
//       {"123ABC123DEF123", "", {"123ABC123DEF123"}},
//       {"", "123", {""}},
//       {"ABC123DEF", "123", {"ABC", "DEF"}},
//       {"123ABC123DEF", "123", {"", "ABC", "DEF"}},
//       {"ABC123DEF123", "123", {"ABC", "DEF", ""}},
//       {"123ABC123DEF123", "123", {"", "ABC", "DEF", ""}},
//       {"123ABC123123DEF123", "123", {"", "ABC", "", "DEF", ""}},
//       {"123123123", "123", {"", "", "", ""}},
//   };
//
//   for (auto [query, sep, expected] : input_string) {
//     vector<string> result{};
//     split<string, defs>(query.begin(), query.end(), begin(sep), end(sep),
//                         back_inserter(result));
//     message << std::setw(3) << std::right << ++total << std::setw(40)
//             << std::left << ") split(\"" + query + "\", \"" + sep + "\") ";
//     if (result != expected) {
//       for (const auto &ele : result) {
//         cout << "\"" << ele << "\" ";
//       }
//       cout << endl;
//       ++failed;
//       message << failed_str << "\n";
//     } else
//       message << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   const vector<tuple<vector<int>, vector<int>, vector<vector<int>>>>
//   input_int{
//       {{}, {}, {{}}},
//       {{1, 2, 3}, {}, {{1, 2, 3}}},
//       {{1, 2, 3}, {2}, {{1}, {3}}},
//       {{1, 2, 3, 0, 0, 0, 4, 5, 6}, {0, 0, 0}, {{1, 2, 3}, {4, 5, 6}}},
//       {{0, 0, 0, 1, 2, 3, 0, 0, 0, 4, 5, 6},
//        {0, 0, 0},
//        {{}, {1, 2, 3}, {4, 5, 6}}},
//       {{1, 2, 3, 0, 0, 0, 4, 5, 6, 0, 0, 0},
//        {0, 0, 0},
//        {{1, 2, 3}, {4, 5, 6}, {}}},
//       {{0, 0, 0, 1, 2, 3, 0, 0, 0, 4, 5, 6, 0, 0, 0},
//        {0, 0, 0},
//        {{}, {1, 2, 3}, {4, 5, 6}, {}}},
//   };
//
//   for (auto [query, sep, expected] : input_int) {
//     vector<vector<int>> result{};
//     split<vector<int>, defs>(query.begin(), query.end(), begin(sep),
//     end(sep),
//                              back_inserter(result));
//     message << std::setw(3) << std::right << ++total << ") ";
//     if (result != expected) {
//       ++failed;
//       message << failed_str << "\n";
//     } else
//       message << passed_str << "\n";
//   }
//
//   message << "\n";
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//

//
// pair_int check_open_file(bool verbose = false) {
//   int total = 0, failed = 0;
//   cout << "~~~ Checking open_file function" << endl;
//
//   sstream message;
//
//   ++total;
//
//   auto file = std::ofstream("test");
//
//   file << "TEST\n";
//   file.close();
//
//   try {
//     ifstream input{};
//     open_file("test", input);
//     string line{};
//     getline(input, line);
//     if (line != "TEST") {
//       ++failed;
//       cout << line << endl;
//     }
//   } catch (const runerror &ex) {
//     cout << ex.what();
//     ++failed;
//   }
//
//   //  fs::remove(path("test"));
//
//   if (failed or verbose)
//     cout << message.str();
//
//   cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;
//
//   return {total, failed};
// }
//
// pair_int EvalComTol::eval_comtol(bool verbose) {
//   int total = 0, failed = 0;
//   cout << gen_framed("Evaluating Common Tools") << "\n\n";
//   update_stats(check_split(verbose), total, failed);
//   update_stats(check_pairify(verbose), total, failed);
//   update_stats(check_only_digits(verbose), total, failed);
//   update_stats(check_str_split(verbose), total, failed);
//   update_stats(check_str_clean_ends(verbose), total, failed);
//   update_stats(check_str_clean(verbose), total, failed);
//   update_stats(check_str_replace(verbose), total, failed);
//   update_stats(check_str_map_fields(verbose), total, failed);
//   update_stats(check_str_join_fields(verbose), total, failed);
//   update_stats(check_open_file(verbose), total, failed);
//   cout << gen_framed("Evaluation Completed") << "\n\n";
//
//   return {total, failed};
// }

int perform_tests(bool verbose) {
  Stats result;

  for (int i = 0; i < 2; ++i) {
    bool verbose = i;
    Stats result;

    cout << gen_framed("Evaluating Common Tools") << "\n\n";

    cout << ">>> Checking Basic functions" << endl;
    result(check_XOR(verbose));
    result(check_split(verbose));

    cout << gen_summary(result, "Evaluation", true) << "\n";
  }

  return result.getFailed();
}

int main() { return perform_tests(true); }
