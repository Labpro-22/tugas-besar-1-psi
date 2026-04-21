#include "models/Petak.hpp"

Petak::Petak(int id, const std::string& name) : id(id), name(name), shortName("   "), colorCode("DF") {}

int Petak::getId() const { return id; }

std::string Petak::getName() const { return name; }

void Petak::setSkin(const std::string& sName, const std::string& cCode) {
    shortName = sName;
    colorCode = cCode;
}

std::string Petak::getShortName() const { return shortName; }
std::string Petak::getColorCode() const { return colorCode; }
