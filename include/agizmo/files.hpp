#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace AGizmo {

namespace Files {

using std::ifstream;
using std::string;

inline void open_file(const string &file_name, ifstream &stream) {
  stream = ifstream(file_name);
  if (!stream.is_open())
    throw std::runtime_error{"Can't open '" + file_name + "'\n"};
}

} // namespace Files

} // namespace AGizmo
