#include "models/Street.hpp"

Street::Street(int id, const std::string& name, int hargaBeli, const std::vector<int>& rentPrices, int housePrice)
    : PetakProperti(id, name, hargaBeli), rentPrices(rentPrices), houseCount(0), housePrice(housePrice) {}

int Street::getHouseCount() const { return houseCount; }

void Street::addHouse() {
    if (houseCount < 5) houseCount++; // Max 5 houses (where 5 = hotel)
}

int Street::getHousePrice() const { return housePrice; }

int Street::getSewa(int /*diceRoll*/) const {
    if (isMortgaged) return 0;
    
    // rentPrices should map: 0->base, 1->1 house, ..., 5->hotel
    if (houseCount >= 0 && houseCount < static_cast<int>(rentPrices.size())) {
        return rentPrices[houseCount];
    }
    return 0;
}
