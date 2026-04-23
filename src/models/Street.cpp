#include "models/Street.hpp"

Street::Street(int id, const std::string& name, const std::string& colorGroup,
               int hargaBeli, const std::vector<int>& rentPrices,
               int housePrice, int hotelPrice)
    : PetakProperti(id, name, hargaBeli),
      colorGroup(colorGroup), rentPrices(rentPrices),
      houseCount(0), isHotelBuilt(false),
      housePrice(housePrice), hotelPrice(hotelPrice),
      festivalMultiplier(1), festivalDuration(0) {}

std::string Street::getColorGroup() const { return colorGroup; }
int Street::getHouseCount() const { return houseCount; }
bool Street::getIsHotel() const { return isHotelBuilt; }
const std::vector<int>& Street::getRentPrices() const { return rentPrices; }

bool Street::canBuildHouse() const {
    return !isHotelBuilt && houseCount < 4;
}

bool Street::canUpgradeToHotel() const {
    return !isHotelBuilt && houseCount == 4;
}

void Street::buildHouse() {
    if (canBuildHouse()) houseCount++;
}

void Street::upgradeToHotel() {
    if (canUpgradeToHotel()) isHotelBuilt = true;
}

void Street::demolishAllBuildings() {
    houseCount = 0;
    isHotelBuilt = false;
}

int Street::getHousePrice() const { return housePrice; }
int Street::getHotelPrice() const { return hotelPrice; }

int Street::getTotalBuildingCost() const {
    if (isHotelBuilt) return 4 * housePrice + hotelPrice;
    return houseCount * housePrice;
}

int Street::getFestivalMultiplier() const { return festivalMultiplier; }
int Street::getFestivalDuration() const   { return festivalDuration; }

void Street::activateFestival() {
    if (festivalMultiplier < 8) festivalMultiplier *= 2;
    festivalDuration = 3;
}

void Street::decrementFestival() {
    if (festivalDuration > 0) {
        festivalDuration--;
        if (festivalDuration == 0) festivalMultiplier = 1;
    }
}

void Street::setFestivalState(int mult, int dur) {
    festivalMultiplier = mult;
    festivalDuration   = dur;
}

void Street::restoreBuildingState(int houses, bool hotel) {
    houseCount    = houses;
    isHotelBuilt  = hotel;
}

int Street::getSewa(int /*diceRoll*/) const {
    if (isMortgaged) return 0;
    int level = isHotelBuilt ? 5 : houseCount;
    int base  = (level >= 0 && level < static_cast<int>(rentPrices.size()))
                ? rentPrices[level] : 0;
    return base * festivalMultiplier;
}
