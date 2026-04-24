#ifndef COLORREGISTRY_HPP
#define COLORREGISTRY_HPP

#include <string>

class ColorRegistry {
public:
  static std::string getFullName(const std::string &code);
  static std::string getAnsiColor(const std::string &code);
};

#endif
