#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Street.hpp"
#include "models/Utility.hpp"
#include <iomanip>
#include <iostream>
#include <string>

#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

PetakProperti::PetakProperti(int id, const std::string &name, int hargaBeli)
    : Petak(id, name), owner(nullptr), hargaBeli(hargaBeli),
      isMortgaged(false) {}

Player *PetakProperti::getOwner() const { return owner; }
void PetakProperti::setOwner(Player *newOwner) { owner = newOwner; }

int PetakProperti::getHargaBeli() const { return hargaBeli; }

bool PetakProperti::getIsMortgaged() const { return isMortgaged; }
void PetakProperti::setMortgaged(bool status) { isMortgaged = status; }

void PetakProperti::gadai() { isMortgaged = true; }

void PetakProperti::tebus() { isMortgaged = false; }

void PetakProperti::lelang() {
  owner = nullptr;
  isMortgaged = false;
}

void PetakProperti::injak(Player &p, IGameUI &ui, int diceRoll) {
  if (owner != nullptr && owner != &p) {
    ui.showMessage("Kamu mendarat di " + getName() + " (" + getShortName() +
                   "), milik " + owner->getName() + "!\n");

    if (isMortgaged) {
      ui.showMessage(
          "Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
    } else {
      int rent = getSewa(diceRoll);

      std::string kondisi = "Tanah Kosong";
      if (auto street = dynamic_cast<Street *>(this)) {
        if (street->isHotelBuilt())
          kondisi = "Hotel";
        else if (street->getHouseCount() > 0)
          kondisi = std::to_string(street->getHouseCount()) + " rumah";
      } else if (dynamic_cast<Railroad *>(this)) {
        kondisi = std::to_string(owner->getRailroadCount()) + " Stasiun";
      } else if (dynamic_cast<Utility *>(this)) {
        int count = owner->getUtilityCount();
        kondisi = std::to_string(count) + " Utilitas (Dadu x " +
                  (count == 1 ? "4" : "10") + ")";
      }

      ui.showMessage("Kondisi     : " + kondisi);
      ui.showMessage("Sewa        : " + formatUang(rent) + "\n");

      //FITUR 7: BAYAR SEWA
      if (p.getMoney() >= rent) {
        int pMoneyBefore = p.getMoney();
        int oMoneyBefore = owner->getMoney();
        p.reduceMoney(rent);
        owner->addMoney(rent);

        ui.showMessage("Uang kamu       : " + formatUang(pMoneyBefore) +
                       " -> " + formatUang(p.getMoney()));
        ui.showMessage("Uang " + owner->getName() + " : " +
                       formatUang(oMoneyBefore) + " -> " +
                       formatUang(owner->getMoney()));
      } else {
        ui.showMessage("---");
        ui.showMessage("Kamu tidak mampu membayar sewa penuh! (" +
                       formatUang(rent) + ")");
        ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));
        ui.showMessage("// Alur dilanjutkan ke Kebangkrutan");

        int pMoneyBefore = p.getMoney();
        owner->addMoney(pMoneyBefore);
        p.reduceMoney(rent);
      }
    }
  }
}
