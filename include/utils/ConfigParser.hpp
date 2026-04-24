#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <memory>
#include <string>
#include <vector>

class Petak;

class ConfigParser {
public:
  static std::vector<std::unique_ptr<Petak>>
  loadBoardConfig(const std::string &filename);
};

#endif
