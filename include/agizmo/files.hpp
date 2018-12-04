#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace AGizmo::Files {

using std::getline;
using std::ifstream;
using std::istream;
using std::ostream;
using std::string;
using std::string_view;
using opt_str = std::optional<string>;
using std::nullopt;
using runerror = std::runtime_error;
using std::make_unique;
using std::unique_ptr;

// inline void open_file(const string &file_name, ifstream &stream) {
inline void open_file(string_view file_name, ifstream &stream) {
  stream = ifstream(file_name.data());
  if (!stream.is_open())
    throw runerror{"Can't open '" + string(file_name.data()) + "'\n"};
}

class FileReader {
 private:
  std::unique_ptr<std::istream> input{nullptr};
  string file_name{};
  string line{};

 public:
  FileReader() = default;

  FileReader(string_view file_name) : file_name{file_name} {
    open(this->file_name);
  }

  FileReader(istream &stream) { open(stream); }

  ~FileReader() {
    std::cerr << "~Filereader()\n";
    close();
  }

  void close() {
    line = "";
    input.release();
  }
  void open(string_view file_name) {
    close();

    ifstream file_input;
    open_file(file_name, file_input);
    std::cerr << "FILESTREAM\n";
    input = std::make_unique<std::ifstream>(std::move(file_input));
  }

  void open(istream &stream) {
    std::cerr << "STDIN\n";
    close();
    this->input = make_unique<istream>(stream.rdbuf());
  }

  string getLine() const { return line; }
  string str() const { return getLine(); }
  [[nodiscard]] bool good() const noexcept { return input->good(); }

  friend std::ostream &operator<<(ostream &stream, const FileReader &reader) {
    return stream << reader.str();
  }

  bool readLine(const string &skip = {}) {
    if (skip.empty()) {
      std::cerr << "READLINE\n";
      std::cerr << "LINE: " << line << "\n";
      //      std::cerr << static_cast<bool>(this->input) << "\n";
      std::cerr << "READLINE\n";
      getline(*input, line);

    } else
      while (getline(*input, line) && line.find_first_of(skip) == 0) continue;

    return good();
  }

  bool readLine(const int skip) {
    if (!skip)
      getline(*input, line);
    else {
      for (int i = 0; i < skip; ++i) {
        getline(*input, line);
        if (!input) break;
      }
    }

    return good();
  }

  bool readLineInto(string &external, const string &skip = {}) {
    if (skip.empty())
      getline(*input, external);
    else
      while (getline(*input, external) && external.find_first_of(skip) == 0)
        continue;
    return good();
  }

  bool readLineInto(string &external, int skip) {
    for (int i = 0; i < skip; ++i) {
      getline(*input, external);
      if (!input) break;
    }
    return good();
  }

  opt_str operator()(const string &skip = {}) {
    if (readLine(skip))
      return line;
    else
      return nullopt;
  }

  opt_str operator()(const int skip) {
    if (readLine(skip))
      return line;
    else
      return nullopt;
  }

  bool setLineToMatch(const string &match) {
    do {
      getline(*input, line);
      if (line == match) return true;
    } while (good());

    return false;
  }
};

}  // namespace AGizmo::Files
