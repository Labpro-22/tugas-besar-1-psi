#ifndef STREET_HPP
#define STREET_HPP

#include "models/PetakProperti.hpp"
#include <string>
#include <vector>

class Street : public PetakProperti {
private:
  std::string colorGroup;
  std::vector<int> rentPrices;
  int houseCount;
  bool isHotelBuilt_field;
  int housePrice;
  int hotelPrice;
  int festivalMultiplier;
  int festivalDuration;

public:
  Street(int id, const std::string &name, const std::string &colorGroup,
         int hargaBeli, const std::vector<int> &rentPrices, int housePrice,
         int hotelPrice);

  std::string getColorGroup() const;
  int getHouseCount() const;
  bool isHotelBuilt() const;
  const std::vector<int> &getRentPrices() const;

  bool canBuildHouse() const;
  bool canUpgradeToHotel() const;
  void buildHouse();
  void upgradeToHotel();
  void demolishAllBuildings();

  int getHousePrice() const;
  int getHotelPrice() const;
  int getTotalBuildingCost() const;

  int  getFestivalMultiplier() const;
  int  getFestivalDuration()   const;
  void activateFestival();
  void decrementFestival();
  void setFestivalState(int mult, int dur);
  void restoreBuildingState(int houses, bool hotel);

  int getSewa(int diceRoll = 0) const override;

  std::string getGroupName() const override;
  int getTotalWorth() const override;
  void printDeed(IGameUI &ui) const override;
};

#endif
