#include "utils/ColorRegistry.hpp"

std::string ColorRegistry::getFullName(const std::string &code) {
  if (code == "CK")
    return "COKLAT";
  if (code == "BM")
    return "BIRU MUDA";
  if (code == "PK")
    return "MERAH MUDA";
  if (code == "OR")
    return "ORANGE";
  if (code == "MR")
    return "MERAH";
  if (code == "KN")
    return "KUNING";
  if (code == "HJ")
    return "HIJAU";
  if (code == "BT")
    return "BIRU TUA";
  if (code == "AB")
    return "ABU-ABU";
  if (code == "DF")
    return "DEFAULT";
  return code;
}

std::string ColorRegistry::getAnsiColor(const std::string &code) {
  if (code == "CK")
    return "\033[38;5;130m";
  if (code == "BM")
    return "\033[96m";
  if (code == "PK")
    return "\033[95m";
  if (code == "AB")
    return "\033[90m";
  if (code == "OR")
    return "\033[38;5;208m";
  if (code == "MR")
    return "\033[91m";
  if (code == "KN")
    return "\033[93m";
  if (code == "HJ")
    return "\033[92m";
  if (code == "BT")
    return "\033[94m";
  if (code == "DF")
    return "\033[97m";
  return "\033[0m";
}
