#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include "models/Petak.hpp"

class ConfigParser {
public:
    // Parses a CSV or TXT file to construct the monopoly board with polymorphic objects
    static std::vector<std::unique_ptr<Petak>> loadBoardConfig(const std::string& filename);
};

#endif
