#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace DataAccess {

    std::vector<std::map<std::string, std::string>> ConfigParser::parseFile(const std::string& filepath) {
        std::vector<std::map<std::string, std::string>> parsedData;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            throw std::runtime_error("Gagal membuka file config: " + filepath);
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '\r' || line[0] == '#') {
                continue;
            }
            parsedData.push_back(parseLine(line));
        }

        return parsedData;
    }

    std::map<std::string, std::string> ConfigParser::parseLine(const std::string& line) {
        std::map<std::string, std::string> lineData;
        std::stringstream ss(line);
        std::string pair;

        while (std::getline(ss, pair, ',')) {
            size_t delimiterPos = pair.find(':');
            if (delimiterPos == std::string::npos) {
                delimiterPos = pair.find('=');
            }

            if (delimiterPos != std::string::npos) {
                std::string key = pair.substr(0, delimiterPos);
                std::string value = pair.substr(delimiterPos + 1);

                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                lineData[key] = value;
            }
        }
        return lineData;
    }

}
