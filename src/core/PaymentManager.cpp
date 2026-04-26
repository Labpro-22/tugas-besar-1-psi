#include "core/PaymentManager.hpp"
#include "core/AuctionManager.hpp"
#include "models/Petak.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Street.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

#include <algorithm>
#include <sstream>

void PaymentManager::processPayment(Player &debtor, Player *creditor, int amount,
                                    IGameUI &ui,
                                    std::vector<std::unique_ptr<Petak>> &board,
                                    std::vector<Player> &players,
                                    const std::string &reason) {
  if (amount <= 0) return;

  if (debtor.isShieldActive()) {
      ui.showMessage("[SHIELD ACTIVE]: Efek ShieldCard melindungi Anda!");
      ui.showMessage("Tagihan M" + std::to_string(amount) + " dibatalkan. Uang Anda tetap: M" + std::to_string(debtor.getMoney()) + ".");
      return;
  }

  if (debtor.getDiscount() > 0) {
      int discountPct = debtor.getDiscount();
      int discountVal = (amount * discountPct) / 100;
      ui.showMessage("[DISCOUNT ACTIVE]: Diskon " + std::to_string(discountPct) + "% memotong tagihan sebesar M" + std::to_string(discountVal) + ".");
      amount -= discountVal;
  }

  if (debtor.getMoney() >= amount) {
    int before = debtor.getMoney();
    debtor.reduceMoney(amount);
    if (creditor) {
      creditor->addMoney(amount);
    }
    if (!reason.empty()) {
        ui.showMessage(reason);
    }
    ui.showMessage("Uang " + debtor.getName() + " : " + formatUang(before) +
                   " -> " + formatUang(debtor.getMoney()));
    return;
  }

  ui.showMessage("Kamu tidak dapat membayar " + formatUang(amount) + " kepada " +
                 (creditor ? creditor->getName() : "Bank") + "!");
  ui.showMessage("Uang kamu : " + formatUang(debtor.getMoney()));
  ui.showMessage("Total kewajiban : " + formatUang(amount));
  ui.showMessage("Kekurangan : " + formatUang(amount - debtor.getMoney()));

  while (debtor.getMoney() < amount && debtor.getStatus() != PlayerStatus::BANKRUPT) {
    int cash = debtor.getMoney();
    int potential = cash;
    std::vector<PetakProperti*> sellable;
    std::vector<PetakProperti*> mortgageable;

    for (auto *prop : debtor.getOwnedProperties()) {
      if (!prop->getIsMortgaged()) {
        int sellValue = prop->getHargaBeli();
        if (auto *street = dynamic_cast<Street*>(prop)) {
            sellValue += street->getTotalBuildingCost() / 2;
        }
        potential += sellValue;
        sellable.push_back(prop);
        
        potential += prop->getHargaBeli() / 2; 
        mortgageable.push_back(prop);
      }
    }

    potential = cash;
    for (auto *prop : debtor.getOwnedProperties()) {
        if (!prop->getIsMortgaged()) {
            int sellValue = prop->getHargaBeli();
            if (auto *street = dynamic_cast<Street*>(prop)) {
                sellValue += street->getTotalBuildingCost() / 2;
            }
            potential += sellValue;
        }
    }

    ui.showMessage("");
    ui.showMessage("Estimasi dana maksimum dari likuidasi:");
    ui.showMessage("  Total potensi : " + formatUang(potential));

    if (potential >= amount) {
      ui.showMessage("Dana likuidasi dapat menutup kewajiban.");
      ui.showMessage("Kamu wajib melikuidasi aset untuk membayar.");
      ui.showMessage("=== Panel Likuidasi ===");
      ui.showMessage("Uang kamu saat ini: " + formatUang(debtor.getMoney()) + " | Kewajiban: " + formatUang(amount));
      
      int index = 1;
      std::vector<std::pair<int, PetakProperti*>> options;
      
      ui.showMessage("[Jual ke Bank]");
      for (auto* prop : sellable) {
          int sellValue = prop->getHargaBeli();
          int buildingSell = 0;
          if (auto *street = dynamic_cast<Street*>(prop)) {
              buildingSell = street->getTotalBuildingCost() / 2;
              sellValue += buildingSell;
          }
          std::string buildingStr = buildingSell > 0 ? " (termasuk bangunan: " + formatUang(buildingSell) + ")" : "";
          ui.showMessage(std::to_string(index) + ". " + prop->getName() + " (" + prop->getShortName() + ") Harga Jual: " + formatUang(sellValue) + buildingStr);
          options.push_back({1, prop});
          index++;
      }
      
      ui.showMessage("[Gadaikan]");
      for (auto* prop : mortgageable) {
          ui.showMessage(std::to_string(index) + ". " + prop->getName() + " (" + prop->getShortName() + ") Nilai Gadai: " + formatUang(prop->getHargaBeli() / 2));
          options.push_back({2, prop});
          index++;
      }
      
      std::string choiceStr = ui.promptInput("Pilih aksi (0 jika sudah cukup): ");
      int choice = 0;
      try { choice = std::stoi(choiceStr); } catch (...) {}
      
      if (choice == 0) {
          if (debtor.getMoney() >= amount) {
              break;
          } else {
              ui.showMessage("Uang belum cukup! Wajib melikuidasi.");
          }
      } else if (choice > 0 && choice <= (int)options.size()) {
          auto opt = options[choice - 1];
          int actionType = opt.first;
          PetakProperti* prop = opt.second;
          
          if (actionType == 1) {
              int sellValue = prop->getHargaBeli();
              if (auto *street = dynamic_cast<Street*>(prop)) {
                  sellValue += street->getTotalBuildingCost() / 2;
                  street->demolishAllBuildings();
              }
              debtor.addMoney(sellValue);
              
              debtor.removeProperty(prop);
              prop->lelang();
              
              ui.showMessage(prop->getName() + " terjual ke Bank. Kamu menerima " + formatUang(sellValue) + ".");
          } else if (actionType == 2) {
              prop->gadai();
              debtor.addMoney(prop->getHargaBeli() / 2);
              ui.showMessage(prop->getName() + " digadaikan. Kamu menerima " + formatUang(prop->getHargaBeli() / 2) + ".");
          }
          
      } else {
          ui.showMessage("Pilihan tidak valid.");
      }

    } else {
      ui.showMessage("Tidak cukup untuk menutup kewajiban " + formatUang(amount) + ".");
      ui.showMessage(debtor.getName() + " dinyatakan BANGKRUT!");
      ui.showMessage("Kreditor: " + std::string(creditor ? creditor->getName() : "Bank"));
      
      debtor.setStatus(PlayerStatus::BANKRUPT);
      
      if (creditor) {
          ui.showMessage("Pengalihan aset ke " + creditor->getName() + ":");
          ui.showMessage("- Uang tunai sisa : " + formatUang(debtor.getMoney()));
          creditor->addMoney(debtor.getMoney());
          
          for (auto* prop : debtor.getOwnedProperties()) {
              std::string status = prop->getIsMortgaged() ? "MORTGAGED [M]" : "OWNED";
              if (auto *street = dynamic_cast<Street*>(prop)) {
                  if (street->isHotelBuilt()) status += " (1 hotel)";
                  else if (street->getHouseCount() > 0) status += " (" + std::to_string(street->getHouseCount()) + " rumah)";
              }
              ui.showMessage("- " + prop->getName() + " (" + prop->getShortName() + ") [" + status + "]");
              
              prop->setOwner(creditor);
              creditor->addProperty(prop);
          }
          debtor.clearProperties();
          ui.showMessage(creditor->getName() + " menerima semua aset " + debtor.getName() + ".");
      } else {
          ui.showMessage("Uang sisa " + formatUang(debtor.getMoney()) + " diserahkan ke Bank.");
          ui.showMessage("Seluruh properti dikembalikan ke status BANK.");
          ui.showMessage("Bangunan dihancurkan - stok dikembalikan ke Bank.");
          
          std::vector<PetakProperti*> propsToAuction = debtor.getOwnedProperties();
          ui.showMessage("Properti akan dilelang satu per satu:");
          for (auto* prop : propsToAuction) {
              ui.showMessage("- Lelang: " + prop->getName() + " (" + prop->getShortName() + ")");
              if (auto *street = dynamic_cast<Street*>(prop)) {
                  street->demolishAllBuildings();
              }
              prop->lelang();
              AuctionManager::startAuction(prop, &debtor, players, ui);
          }
          debtor.clearProperties();
      }
      ui.showMessage(debtor.getName() + " telah keluar dari permainan.");
      return;
    }
  }

  if (debtor.getStatus() != PlayerStatus::BANKRUPT) {
      ui.showMessage("Kewajiban " + formatUang(amount) + " terpenuhi. Membayar...");
      int before = debtor.getMoney();
      debtor.reduceMoney(amount);
      if (creditor) {
          int cBefore = creditor->getMoney();
          creditor->addMoney(amount);
          ui.showMessage("Uang " + creditor->getName() + ": " + formatUang(cBefore) + " -> " + formatUang(creditor->getMoney()));
      }
      ui.showMessage("Uang " + debtor.getName() + ": " + formatUang(before) + " -> " + formatUang(debtor.getMoney()));
  }
}
