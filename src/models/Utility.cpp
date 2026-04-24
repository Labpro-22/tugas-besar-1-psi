#include "models/Utility.hpp"
#include "models/Player.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

Utility::Utility(int id, const std::string &name, int hargaBeli)
    : PetakProperti(id, name, hargaBeli) {}

int Utility::getSewa(int diceRoll) const {
  if (isMortgaged || !owner)
    return 0;

  int count = owner->getUtilityCount();

  if (count == 1) {
    return diceRoll * 4;
  } else if (count >= 2) {
    return diceRoll * 10;
  }

  return 0;
}

std::string Utility::getGroupName() const { return "UTILITAS"; }

int Utility::getTotalWorth() const { return hargaBeli; }

void Utility::printDeed(IGameUI &ui) const {
  int nilaiGadai = hargaBeli / 2;
  ui.showMessage("| Harga Beli        : " + formatUang(hargaBeli));
  ui.showMessage("| Nilai Gadai       : " + formatUang(nilaiGadai));
  ui.showMessage("+--------------------------------+");
  ui.showMessage("| Sewa 1 Utilitas   : Dadu x 4");
  ui.showMessage("| Sewa 2 Utilitas   : Dadu x 10");
}
