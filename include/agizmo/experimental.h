#pragma once

namespace AGizmo {

namespace Experimental {

// inline string str_mark(const string &source,
//                       const map<int, std::string> &index) {
//  if (!source.length())
//    return "";
//  else if (index.empty())
//    return source;
//  else {
//    string result = source;
//    auto length{source.length()};
//    size_t last{0};

//    for (auto[key, value] : index) {
//      if (key < 0)
//        continue;
//      if (key >= length)
//        break;
//      result.insert(key + last + 1, value);
//      last += value.size();
//    }
//    return result;
//  }
//}

} // namespace ExpTol
