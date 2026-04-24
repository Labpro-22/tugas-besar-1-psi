#include "models/Street.hpp"
#include "utils/ColorRegistry.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

Street::Street(int id, const std::string &name, const std::string &colorGroup,
               int hargaBeli, const std::vector<int> &rentPrices,
               int housePrice, int hotelPrice)
    : PetakProperti(id, name, hargaBeli), colorGroup(colorGroup),
      rentPrices(rentPrices), houseCount(0), isHotelBuilt_field(false),
      housePrice(housePrice), hotelPrice(hotelPrice) {}

std::string Street::getColorGroup() const { return colorGroup; }
int Street::getHouseCount() const { return houseCount; }
bool Street::isHotelBuilt() const { return isHotelBuilt_field; }
const std::vector<int> &Street::getRentPrices() const { return rentPrices; }

bool Street::canBuildHouse() const {
  return !isHotelBuilt_field && houseCount < 4;
}

bool Street::canUpgradeToHotel() const {
  return !isHotelBuilt_field && houseCount == 4;
}

void Street::buildHouse() {
  if (canBuildHouse())
    houseCount++;
}

void Street::upgradeToHotel() {
  if (canUpgradeToHotel())
    isHotelBuilt_field = true;
}

void Street::demolishAllBuildings() {
  houseCount = 0;
  isHotelBuilt_field = false;
}

int Street::getHousePrice() const { return housePrice; }
int Street::getHotelPrice() const { return hotelPrice; }

int Street::getTotalBuildingCost() const {
  if (isHotelBuilt_field)
    return 4 * housePrice + hotelPrice;
  return houseCount * housePrice;
}

int Street::getSewa(int /*diceRoll*/) const {
  if (isMortgaged)
    return 0;
  int level = isHotelBuilt_field ? 5 : houseCount;
  if (level >= 0 && level < static_cast<int>(rentPrices.size())) {
    return rentPrices[level];
  }
  return 0;
}

std::string Street::getGroupName() const {
  return ColorRegistry::getFullName(colorGroup);
}

int Street::getTotalWorth() const { return hargaBeli + getTotalBuildingCost(); }

void Street::printDeed(IGameUI &ui) const {
  int nilaiGadai = hargaBeli / 2;
  ui.showMessage("| Harga Beli        : " + formatUang(hargaBeli));
  ui.showMessage("| Nilai Gadai       : " + formatUang(nilaiGadai));
  ui.showMessage("+--------------------------------+");

  ui.showMessage("| Sewa (unimproved) : " +
                 (rentPrices.size() > 0 ? formatUang(rentPrices[0]) : "M0"));
  ui.showMessage("| Sewa (1 rumah)    : " +
                 (rentPrices.size() > 1 ? formatUang(rentPrices[1]) : "M0"));
  ui.showMessage("| Sewa (2 rumah)    : " +
                 (rentPrices.size() > 2 ? formatUang(rentPrices[2]) : "M0"));
  ui.showMessage("| Sewa (3 rumah)    : " +
                 (rentPrices.size() > 3 ? formatUang(rentPrices[3]) : "M0"));
  ui.showMessage("| Sewa (4 rumah)    : " +
                 (rentPrices.size() > 4 ? formatUang(rentPrices[4]) : "M0"));
  ui.showMessage("| Sewa (hotel)      : " +
                 (rentPrices.size() > 5 ? formatUang(rentPrices[5]) : "M0"));
  ui.showMessage("+--------------------------------+");
  ui.showMessage("| Harga Rumah       : " + formatUang(housePrice));
  ui.showMessage("| Harga Hotel       : " + formatUang(hotelPrice));
}
