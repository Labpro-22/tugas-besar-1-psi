#ifndef FORMATHELPER_HPP
#define FORMATHELPER_HPP

#include <string>

inline std::string formatUang(int amount) {
  std::string s = std::to_string(amount);
  int n = s.length();
  for (int i = n - 3; i > 0; i -= 3) {
    s.insert(i, ".");
  }
  return "M" + s;
}

#endif
