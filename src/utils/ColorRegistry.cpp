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
    return "\033[38;5;130m"; // Coklat
  if (code == "BM")
    return "\033[96m"; // Biru Muda
  if (code == "PK")
    return "\033[95m"; // Pink
  if (code == "AB")
    return "\033[90m"; // Abu-abu
  if (code == "OR")
    return "\033[38;5;208m"; // Orange
  if (code == "MR")
    return "\033[91m"; // Merah
  if (code == "KN")
    return "\033[93m"; // Kuning
  if (code == "HJ")
    return "\033[92m"; // Hijau
  if (code == "BT")
    return "\033[94m"; // Biru Tua
  if (code == "DF")
    return "\033[97m"; // Putih/Default
  return "\033[0m";    // Default
}
