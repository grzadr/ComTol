#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace AGizmo::Args {

using std::cerr;
using std::optional;
using std::string;
using std::vector;

struct Flag {
  optional<string> name{};
  string value{};

  Flag() = default;
};

class Arguments {
 private:
  vector<Flag> args{};

 public:
  Arguments() = delete;
  Arguments(int argc, char* argv[]) {
    string name{};
    for (int i = 1; i <= argc; ++i) {
      if (name.empty()) {
        string temp = argv[i];
      } else
        args.emplace_back(name, argv[i]);

      cerr << argv[i] << "\n";
    }
  }
};

}  // namespace AGizmo::Args
