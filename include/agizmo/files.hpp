#pragma once

#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace AGizmo::Files {

using std::getline;
using std::ifstream;
using std::ostream;
using std::string;
using std::string_view;
using opt_str = std::optional<string>;
using std::nullopt;
using runerror = std::runtime_error;

// inline void open_file(const string &file_name, ifstream &stream) {
inline void open_file(string_view file_name, ifstream &stream) {
  stream = ifstream(file_name.data());
  if (!stream.is_open())
    throw runerror{"Can't open '" + string(file_name.data()) + "'\n"};
}

class FileReader {
 private:
  ifstream input;
  string file_name, line{};

 public:
  FileReader() = delete;

  FileReader(string_view file_name) : file_name{file_name} {
    open(this->file_name);
  }

  ~FileReader() { close(); }

  void close() { input.close(); }
  void open(string_view file_name) {
    close();
    open_file(file_name, input);
  }

  string getLine() const { return line; }
  string str() const { return getLine(); }
  bool good() { return input.good(); }

  friend std::ostream &operator<<(ostream &stream, const FileReader &reader) {
    return stream << reader.str();
  }

  bool readLine(const string &skip = {}) {
    if (skip.empty())
      getline(input, line);
    else
      while (getline(input, line) && line.find_first_of(skip) == 0) continue;

    return input.good();
  }

  bool readLine(int skip) {
    for (int i = 0; i < skip; ++i) {
      getline(input, line);
      if (!input) break;
    }
    return input.good();
  }

  bool readLineInto(string &external, const string &skip = {}) {
    if (skip.empty())
      getline(input, external);
    else
      while (getline(input, external) && external.find_first_of(skip) == 0)
        continue;
    return input.good();
  }

  bool readLineInto(string &external, int skip) {
    for (int i = 0; i < skip; ++i) {
      getline(input, external);
      if (!input) break;
    }
    return input.good();
  }

  opt_str operator()(const string &skip = {}) {
    if (readLine(skip))
      return line;
    else
      return nullopt;
  }
};

}  // namespace AGizmo::Files
