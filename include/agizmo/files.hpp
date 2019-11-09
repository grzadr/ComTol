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
using std::wifstream;
using std::wistream;
using std::wostream;
using std::wstring;
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

// inline void open_file(const string &file_name, ifstream &stream) {
inline void open_file(string_view file_name, wifstream &stream) {
  stream = wifstream(file_name.data());
  if (!stream.is_open())
    throw runerror{"Can't open '" + string(file_name.data()) + "'\n"};
}

class FileReaderWide {
private:
  std::unique_ptr<std::wistream> input{nullptr};
  string file_name{};
  wstring line{};
  long line_num{0};

public:
  FileReaderWide() = delete;

  FileReaderWide(const string &file_name) : file_name{file_name} {
    open(this->file_name);
  }

  FileReaderWide(wistream &stream) { open(stream); }

  void close() {
    line.clear();
    input.release();
  }

  void open(string_view file_name) {
    close();

    wifstream file_input;
    open_file(file_name, file_input);
    input = std::make_unique<std::wifstream>(std::move(file_input));
  }

  void open(wistream &stream) {
    close();
    this->input = make_unique<wistream>(stream.rdbuf());
  }

  wstring getLine() const { return line; }
  long getLineNum() const { return line_num; }
  wstring str() const { return getLine(); }
  [[nodiscard]] bool good() const noexcept { return input->good(); }

  friend std::wostream &operator<<(wostream &stream,
                                   const FileReaderWide &reader) {
    return stream << reader.str();
  }

  //  bool readLine(const string &skip = {}) {
  //    if (skip.empty()) {
  //      getline(*input, line);
  //      ++line_num;
  //    } else {
  //      while (getline(*input, line) && line.find_first_of(skip) == 0) {
  //        ++line_num;
  //        continue;
  //      }
  //      ++line_num;
  //    }

  //    return good();
  //  }

  //  bool readLine(const int skip) {
  //    if (!skip)
  //      readLine();
  //    else {
  //      for (int i = 0; i < skip; ++i) {
  //        getline(*input, line);
  //        if (!input)
  //          break;
  //      }
  //      line_num += skip;
  //    }

  //    return good();
  //  }

  //  bool readLineInto(string &external, const string &skip = {}) {
  //    if (skip.empty())
  //      getline(*input, external);
  //    else
  //      while (getline(*input, external) && external.find_first_of(skip) == 0)
  //        continue;
  //    return good();
  //  }

  //  bool readLineInto(string &external, int skip) {
  //    for (int i = 0; i < skip; ++i) {
  //      getline(*input, external);
  //      if (!input)
  //        break;
  //    }
  //    return good();
  //  }

  //  opt_str operator()(const string &skip = {}) {
  //    if (readLine(skip))
  //      return line;
  //    else
  //      return nullopt;
  //  }

  //  opt_str operator()(const int skip) {
  //    if (readLine(skip))
  //      return line;
  //    else
  //      return nullopt;
  //  }

  //  bool setLineToMatch(const string &match) {
  //    do {
  //      getline(*input, line);
  //      if (line == match)
  //        return true;
  //    } while (good());

  //    return false;
  //  }
};

class FileReader {
private:
  std::unique_ptr<std::istream> input{nullptr};
  string file_name{};
  string line{};
  int line_num{0};

public:
  FileReader() = delete;

  FileReader(const string &file_name) : file_name{file_name} {
    open(this->file_name);
  }

  FileReader(istream &stream) { open(stream); }

  ~FileReader() { close(); }

  void close() {
    line = "";
    input.release();
  }
  void open(string_view file_name) {
    close();

    ifstream file_input;
    open_file(file_name, file_input);
    input = std::make_unique<std::ifstream>(std::move(file_input));
  }

  void open(istream &stream) {
    close();
    this->input = make_unique<istream>(stream.rdbuf());
  }

  string getLine() const { return line; }
  int getLineNum() const { return line_num; }
  string str() const { return getLine(); }
  [[nodiscard]] bool good() const noexcept { return input->good(); }

  friend std::ostream &operator<<(ostream &stream, const FileReader &reader) {
    return stream << reader.str();
  }

  bool readLine(const string &skip = {}) {
    if (skip.empty()) {
      getline(*input, line);
      ++line_num;
    } else {
      while (getline(*input, line) && line.find_first_of(skip) == 0) {
        ++line_num;
        continue;
      }
      ++line_num;
    }

    return good();
  }

  bool readLine(const int skip) {
    if (!skip)
      readLine();
    else {
      for (int i = 0; i < skip; ++i) {
        getline(*input, line);
        if (!input)
          break;
      }
      line_num += skip;
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
      if (!input)
        break;
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
      if (line == match)
        return true;
    } while (good());

    return false;
  }
};

} // namespace AGizmo::Files
