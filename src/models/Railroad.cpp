#include "models/Railroad.hpp"
#include "models/Player.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

Railroad::Railroad(int id, const std::string &name, int hargaBeli, int baseRent)
    : PetakProperti(id, name, hargaBeli), baseRent(baseRent) {}

int Railroad::getSewa(int ) const {
  if (isMortgaged || !owner)
    return 0;

  int count = owner->getRailroadCount();

  if (count == 0)
    return 0;

  int multiplier = 1 << (count - 1);
  return baseRent * multiplier;
}

std::string Railroad::getGroupName() const { return "STASIUN"; }

int Railroad::getTotalWorth() const { return hargaBeli; }

void Railroad::printDeed(IGameUI &ui) const {
  int nilaiGadai = hargaBeli / 2;
  ui.showMessage("| Harga Beli        : " + formatUang(hargaBeli));
  ui.showMessage("| Nilai Gadai       : " + formatUang(nilaiGadai));
  ui.showMessage("+--------------------------------+");
  ui.showMessage("| Sewa 1 Stasiun    : M25");
  ui.showMessage("| Sewa 2 Stasiun    : M50");
  ui.showMessage("| Sewa 3 Stasiun    : M100");
  ui.showMessage("| Sewa 4 Stasiun    : M200");
}
