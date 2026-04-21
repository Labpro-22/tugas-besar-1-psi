#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <map>
#include <vector>

namespace DataAccess {

    class ConfigParser {
    public:
        std::vector<std::map<std::string, std::string>> parseFile(const std::string& filepath);

    private:
        std::map<std::string, std::string> parseLine(const std::string& line);
    };

}

#endif
