#ifndef STREET_HPP
#define STREET_HPP

#include "models/PetakProperti.hpp"
#include <vector>
#include <string>

class Street : public PetakProperti {
private:
    std::string colorGroup;
    std::vector<int> rentPrices;
    int houseCount;
    bool isHotelBuilt;
    int housePrice;
    int hotelPrice;

public:
    Street(int id, const std::string& name, const std::string& colorGroup,
           int hargaBeli, const std::vector<int>& rentPrices,
           int housePrice, int hotelPrice);

    std::string getColorGroup() const;
    int getHouseCount() const;
    bool getIsHotel() const;

    bool canBuildHouse() const;
    bool canUpgradeToHotel() const;
    void buildHouse();
    void upgradeToHotel();
    void demolishAllBuildings();

    int getHousePrice() const;
    int getHotelPrice() const;
    int getTotalBuildingCost() const;

    int getSewa(int diceRoll = 0) const override;
};

#endif
