#include "models/Street.hpp"

Street::Street(int id, const std::string& name, const std::string& colorGroup,
               int hargaBeli, const std::vector<int>& rentPrices,
               int housePrice, int hotelPrice)
    : PetakProperti(id, name, hargaBeli),
      colorGroup(colorGroup), rentPrices(rentPrices),
      houseCount(0), isHotelBuilt(false),
      housePrice(housePrice), hotelPrice(hotelPrice) {}

std::string Street::getColorGroup() const { return colorGroup; }
int Street::getHouseCount() const { return houseCount; }
bool Street::getIsHotel() const { return isHotelBuilt; }

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

int Street::getSewa(int /*diceRoll*/) const {
    if (isMortgaged) return 0;
    int level = isHotelBuilt ? 5 : houseCount;
    if (level >= 0 && level < static_cast<int>(rentPrices.size())) {
        return rentPrices[level];
    }
    return 0;
}
