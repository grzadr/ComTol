#include "basic_test.hpp"

#include <fstream>
#include <iostream>
#include <memory>

#include <numeric>

using std::cout;
using std::endl;

using std::unique_ptr;

using namespace StringFormat;
using namespace StringCompose;
using namespace StringDecompose;

using std::accumulate;
using std::move;
using std::next;
using std::to_string;

Stats check_XOR(bool verbose = false) {
  Stats result;
  sstream message, failure;

  message << "~~~ Checking Basic::XOR\n";

  message << "\nBoolean Test:\n";

  vector<XORWithBool> set_xor_with_bool{
      {{false, false}, false},
      {{true, false}, true},
      {{false, true}, true},
      {{true, true}, false},
  };

  Evaluator xor_with_bool("Basic::XOR", set_xor_with_bool);
  result(xor_with_bool.verify(message, failure));

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  message.clear();
  failure.clear();

  message << "\nChar Test:\n";

  vector<XORWithChar> set_xor_with_char{
      {{0, 0}, false},
      {{0, '+'}, true},
      {{'+', 0}, true},
      {{'+', '+'}, false},
  };

  Evaluator xor_with_char("Basic::XOR", set_xor_with_char);
  result(xor_with_char.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ " << gen_summary(result, "Checking Basic::XOR") << endl;

  return result;
}

Stats check_split(bool verbose = false) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking Basic::split\n";

  message << "\nTesting vector<int> with int separator:\n";

  vector<SplitVectorWithSep> tests_int = {
      {{{}, 2}, {{}}},
      {{{1, 2, 3}, 4}, {{1, 2, 3}}},
      {{{1, 1, 2, 3, 3}, 2}, {{1, 1}, {3, 3}}},
      {{{1, 1, 2}, 2}, {{1, 1}, {}}},
      {{{2, 3, 3}, 2}, {{}, {3, 3}}},
      {{{2}, 2}, {{}, {}}},
      {{{1, 1, 2, 3, 3, 2}, 2}, {{1, 1}, {3, 3}, {}}},
      {{{2, 1, 1, 2, 3, 3}, 2}, {{}, {1, 1}, {3, 3}}},
      {{{2, 1, 1, 2, 3, 3, 2}, 2}, {{}, {1, 1}, {3, 3}, {}}},
      {{{2, 2}, 2}, {{}, {}, {}}},
  };

  Evaluator test_split_int("Basic::split", tests_int);
  result(test_split_int.verify(message, failure));

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  message.clear();
  failure.clear();

  message << "\nTesting vector<int> with "
             "vector<int> separator:\n";

  vector<SplitVectorWithVectorSep> tests = {
      {{{}, {}}, {{}}},
      {{{1, 2, 3}, {}}, {{1, 2, 3}}},
      {{{1, 2, 3}, {4}}, {{1, 2, 3}}},
      {{{1, 1, 2, 3, 3}, {2}}, {{1, 1}, {3, 3}}},
      {{{1, 1, 2}, {2}}, {{1, 1}, {}}},
      {{{2, 3, 3}, {2}}, {{}, {3, 3}}},
      {{{2}, {2}}, {{}, {}}},
      {{{1, 1, 2, 3, 3, 2}, {2}}, {{1, 1}, {3, 3}, {}}},
      {{{2, 1, 1, 2, 3, 3}, {2}}, {{}, {1, 1}, {3, 3}}},
      {{{2, 1, 1, 2, 3, 3, 2}, {2}}, {{}, {1, 1}, {3, 3}, {}}},
      {{{1, 1, 2, 2, 3, 3}, {2, 2}}, {{1, 1}, {3, 3}}},
      {{{2, 2, 1, 1, 2, 2, 3, 3, 2, 2}, {2, 2}}, {{}, {1, 1}, {3, 3}, {}}},
      {{{2, 2, 1, 1, 2, 2, 3, 3}, {2, 2}}, {{}, {1, 1}, {3, 3}}},
      {{{1, 1, 2, 2, 3, 3, 2, 2}, {2, 2}}, {{1, 1}, {3, 3}, {}}},
      {{{1, 1, 2, 2}, {2, 2}}, {{1, 1}, {}}},
      {{{2, 2, 3, 3}, {2, 2}}, {{}, {3, 3}}},
      {{{2, 2}, {2, 2}}, {{}, {}}},
      {{{2, 2}, {2}}, {{}, {}, {}}},
  };

  Evaluator test_split("Basic::split", tests);
  result(test_split.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ " << gen_summary(result, "Checking Basic::split function")
       << endl;

  return result;
}

Stats check_segment(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking Basic::segment\n\nTesting vector<int>:\n";

  vector<SegmentVector> tests = {
      {{{}, 3}, {{}}},
      {{{1, 2}, 0}, {{1, 2}}},
      {{{1, 2}, 3}, {{1, 2}}},
      {{{1, 2, 3}, 3}, {{1, 2, 3}}},
      {{{1, 2, 3, 1, 2, 3}, 3}, {{1, 2, 3}, {1, 2, 3}}},
      {{{1, 2, 3, 1, 2, 3, 1}, 3}, {{1, 2, 3}, {1, 2, 3}, {1}}},
      {{{1, 2, 3, 1, 2}, 3}, {{1, 2, 3}, {1, 2}}},
  };

  Evaluator test_segment("Basic::segment", tests);

  result(test_segment.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ " << gen_summary(result, "Checking Basic::segment function")
       << endl;

  return result;
}

Stats check_merge(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking Basic::merge\n\nTesting vector<int>:\n";

  vector<MergeVector> tests = {
      {{{}, 0}, {}},
      {{{1}, 0}, {1}},
      {{{1, 2}, 0}, {1, 0, 2}},
  };

  Evaluator test_segment("Basic::merge", tests);

  result(test_segment.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ " << gen_summary(result, "Checking Basic::merge function")
       << endl;

  return result;
}

Stats check_only_digits(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::only_digits\n\nTesting strings:\n";

  vector<OnlyDigits> tests = {
      {"0", true},     {"123", true},   {"", false},     {"-1", false},
      {"3.14", false}, {"10E2", false}, {"10e2", false},
  };

  Evaluator test_segment("StringFormat::only_digits", tests);

  result(test_segment.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::only_digits function")
       << endl;

  return result;
}

Stats check_str_to_int(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_to_int\n\nTesting strings:\n";

  vector<StrToInt> tests = {
      {"0", {0}},
      {"000", {0}},
      {"123", {123}},
      {"", {PrintableOptional<int>()}},
      {"A123", {PrintableOptional<int>()}},
      {"123-456", {PrintableOptional<int>()}},
  };

  Evaluator test_segment("StringFormat::str_to_int", tests);

  result(test_segment.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_to_int function")
       << endl;

  return result;
}

Stats check_str_clean_ends(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_clean_ends\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrCleanEnds> tests = {
      {" \t \nA  B \t \n", "A  B"},
      {" \t  \t ", ""},
      {"", ""},
      {"A  B", "A  B"},
      {" \t A  B", "A  B"},
      {"A  B \t ", "A  B"},
  };

  Evaluator test_clean("StringFormat::str_clean_ends", tests);

  result(test_clean.verify(message, failure));

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  message.clear();
  failure.clear();

  message << "\nTesting strings with default parameters:\n";

  vector<StrCleanEndsWithChars> tests_with_chars = {
      {{"\n \t \nA  B\n \t \n", " \n"}, "\t \nA  B\n \t"},
      {{"\t \nA  B\n \t \n", " \n"}, "\t \nA  B\n \t"},
      {{"\n \t \nA  B\n \t", " \n"}, "\t \nA  B\n \t"},
      {{"    ", " \n"}, ""},
      {{"\n\n\n\n", "\n "}, ""},
      {{"\n\n \n", "\n "}, ""},
  };

  Evaluator test_clean_with_chars("StringFormat::str_clean_ends",
                                  tests_with_chars);

  result(test_clean_with_chars.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_clean_ends function")
       << endl;

  return result;
}

Stats check_str_clean(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_clean\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrClean> tests = {
      {"  \nA  B  \n", "A B"},  {"    ", ""},           {"", ""},
      {"A  B", "A B"},          {" \t A  B", "\t A B"}, {"A  B \t ", "A B \t"},
      {"A\t   \tB", "A\t \tB"},
  };

  Evaluator test_clean("StringFormat::str_clean", tests);

  result(test_clean.verify(message, failure));

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  message.clear();
  failure.clear();

  message << "\nTesting strings with default parameters:\n";

  vector<StrCleanWithChars> tests_with_chars = {
      {{"\n \t \nA  B\n \t \n", " \n"}, "\t A B \t"},
      {{"\t \nA  B\n \t \n", " \n"}, "\t A B \t"},
      {{"\n \t \nA  B\n \t", " \n"}, "\t A B \t"},
      {{"    ", " \n"}, ""},
      {{"\n\n\n\n", "\n "}, ""},
      {{"\n\n \n", "\n "}, ""},
  };

  Evaluator test_clean_with_chars("StringFormat::str_clean", tests_with_chars);

  result(test_clean_with_chars.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_clean function")
       << endl;

  return result;
}

Stats check_str_join(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_join\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrJoin> tests = {
      {{1, 2}, "1-2"},
      {{}, ""},
      {{1}, "1"},
  };

  Evaluator test_join("StringFormat::str_join", tests);

  result(test_join.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_join function")
       << endl;

  return result;
}

Stats check_str_reverse(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_reverse\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrReverse> tests = {
      {"", ""},
      {"1-2", "2-1"},
      {"1", "1"},
  };

  Evaluator test_reverse("StringFormat::str_join", tests);

  result(test_reverse.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_reverse function")
       << endl;

  return result;
}

Stats check_str_split(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_split\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrSplit> tests = {
      {{"", ""}, {""}},
      {{"", "_"}, {""}},
      {{"ABC_DEF", ""}, {"ABC_DEF"}},
      {{"ABC_DEF", "_"}, {"ABC", "DEF"}},
      {{"_ABC_DEF", "_"}, {"", "ABC", "DEF"}},
      {{"ABC_DEF_", "_"}, {"ABC", "DEF", ""}},
      {{"_ABC_DEF_", "_"}, {"", "ABC", "DEF", ""}},
      {{"_ABC__DEF_", "_"}, {"", "ABC", "", "DEF", ""}},
      {{"ABC_DEF", "__"}, {"ABC_DEF"}},
      {{"__ABC_DEF", "__"}, {"", "ABC_DEF"}},
      {{"ABC_DEF__", "__"}, {"ABC_DEF", ""}},
      {{"__ABC_DEF__", "__"}, {"", "ABC_DEF", ""}},
      {{"__ABC__DEF__", "__"}, {"", "ABC", "DEF", ""}},
      {{"__ABC____DEF__", "__"}, {"", "ABC", "", "DEF", ""}},
  };

  Evaluator test_split("StringFormat::str_split", tests);

  result(test_split.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_split function")
       << endl;

  return result;
}

Stats check_str_replace(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_replace\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrReplace> tests = {
      {{"", "", ""}, {""}},
      {{"", "*", ""}, {""}},
      {{"", "*", "+"}, {""}},
      {{"", "", "+"}, {""}},
      {{"ABC_DEF", "", ""}, {"ABC_DEF"}},
      {{"ABC_DEF", "", "+"}, {"ABC_DEF"}},
      {{"ABC_DEF", "*", "+"}, {"ABC_DEF"}},
      {{"ABC_DEF", "*", ""}, {"ABC_DEF"}},
      {{"ABC_DEF", "_", ""}, {"ABCDEF"}},
      {{"__ABC__DEF__", "_", ""}, {"ABCDEF"}},
      {{"__ABC__DEF__", "__", ""}, {"ABCDEF"}},
      {{"__ABC__DEF__", "_", "+"}, {"++ABC++DEF++"}},
      {{"__ABC__DEF__", "__", "+"}, {"+ABC+DEF+"}},
      {{"__ABC__DEF__", "_", "++"}, {"++++ABC++++DEF++++"}},
      {{"__ABC__DEF__", "__", "++"}, {"++ABC++DEF++"}},
  };

  Evaluator test_replace("StringFormat::str_split", tests);

  result(test_replace.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_replace function")
       << endl;

  return result;
}

Stats check_join_fields(bool verbose) {
  Stats result;
  sstream message, failure;

  message << "\n~~~ Checking StringFormat::str_replace\n"
          << "\nTesting strings with default parameters:\n";

  vector<StrReplace> tests = {
      {{"", "", ""}, {""}},
      {{"", "*", ""}, {""}},
      {{"", "*", "+"}, {""}},
      {{"", "", "+"}, {""}},
      {{"ABC_DEF", "", ""}, {"ABC_DEF"}},
      {{"ABC_DEF", "", "+"}, {"ABC_DEF"}},
      {{"ABC_DEF", "*", "+"}, {"ABC_DEF"}},
      {{"ABC_DEF", "*", ""}, {"ABC_DEF"}},
      {{"ABC_DEF", "_", ""}, {"ABCDEF"}},
      {{"__ABC__DEF__", "_", ""}, {"ABCDEF"}},
      {{"__ABC__DEF__", "__", ""}, {"ABCDEF"}},
      {{"__ABC__DEF__", "_", "+"}, {"++ABC++DEF++"}},
      {{"__ABC__DEF__", "__", "+"}, {"+ABC+DEF+"}},
      {{"__ABC__DEF__", "_", "++"}, {"++++ABC++++DEF++++"}},
      {{"__ABC__DEF__", "__", "++"}, {"++ABC++DEF++"}},
  };

  Evaluator test_replace("StringFormat::str_split", tests);

  result(test_replace.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ "
       << gen_summary(result, "Checking StringFormat::str_replace function")
       << endl;

  return result;
}

Stats check_open_file(bool verbose = false) {
  Stats result;
  sstream message, failure;
  message << "\n~~~ Checking Files::open_file\n";

  vector<OpenFile> tests = {{"TEST", "TEST"}};

  Evaluator test_open_file("Files::open_file", tests);

  result(test_open_file.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ " << gen_summary(result, "Checking Files::open_file function")
       << endl;

  return result;
}

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

int perform_tests(bool verbose) {
  Stats result;

  cout << gen_framed("Evaluating Common Tools") << "\n";

  cout << "\n>>> Checking Basic functions" << endl;
  result(check_XOR(verbose));
  result(check_split(verbose));
  result(check_segment(verbose));
  result(check_merge(verbose));
  cout << ">>> Done\n";

  cout << "\n>>> Checking String functions" << endl;
  result(check_only_digits(verbose));
  result(check_str_to_int(verbose));
  result(check_str_clean_ends(verbose));
  result(check_str_clean(verbose));
  result(check_str_join(verbose));
  result(check_str_reverse(verbose));
  result(check_str_split(verbose));
  result(check_str_replace(verbose));
  cout << ">>> Done\n";

  cout << "\n>>> Checking Files functions" << endl;
  result(check_open_file(verbose));
  cout << ">>> Done\n";

  cout << "\n" << gen_summary(result, "Evaluation", true) << "\n";

  return result.getFailed();
}

int main() { return perform_tests(true); }

XORWithBool::XORWithBool(pair_bool input, bool expected)
    : BaseTest(input, expected) {
  validate();
}

XORWithChar::XORWithChar(pair_char input, bool expected)
    : BaseTest(input, expected) {
  validate();
}

SplitVectorWithVectorSep::SplitVectorWithVectorSep(
    InputVectorSep<PrintableVector<int>> input, NestedVector<int> expected)
    : BaseTest(input, expected) {
  validate();
}

SplitVectorWithSep::SplitVectorWithSep(InputVectorSep<int> input,
                                       NestedVector<int> expected)
    : BaseTest(input, expected) {
  validate();
}

SegmentVector::SegmentVector(SegmentInput input, NestedVector<int> expected)
    : BaseTest(input, expected) {
  validate();
}

MergeVector::MergeVector(InputVectorSep<int> input,
                         PrintableVector<int> expected)
    : BaseTest(input, expected) {
  validate();
}

OnlyDigits::OnlyDigits(string input, bool expected)
    : BaseTest(input, expected) {
  validate();
}

StrToInt::StrToInt(string input, PrintableOptional<int> expected)
    : BaseTest(input, expected) {
  validate();
}

StrCleanEnds::StrCleanEnds(string input, string expected)
    : BaseTest(input, expected) {
  validate();
}

StrCleanEndsWithChars::StrCleanEndsWithChars(StrCleanWithCharsInput input,
                                             string expected)
    : BaseTest(input, expected) {
  validate();
}

StrClean::StrClean(string input, string expected) : BaseTest(input, expected) {
  validate();
}
StrCleanWithChars::StrCleanWithChars(StrCleanWithCharsInput input,
                                     string expected)
    : BaseTest(input, expected) {
  validate();
}

StrJoin::StrJoin(PrintableVector<int> input, string expected)
    : BaseTest(input, expected) {
  validate();
}

StrReverse::StrReverse(string input, string expected)
    : BaseTest(input, expected) {
  validate();
}

StrSplit::StrSplit(pair_str input, PrintableVector<string> expected)
    : BaseTest(input, expected) {
  validate();
}

StrReplace::StrReplace(StrReplaceInput input, string expected)
    : BaseTest(input, expected) {
  validate();
}

OpenFile::OpenFile(std::string input, std::string expected)
    : BaseTest(input, expected) {
  validate();
}
