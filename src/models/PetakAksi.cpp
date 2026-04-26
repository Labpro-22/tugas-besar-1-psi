#include "models/PetakAksi.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Street.hpp"
#include "models/Utility.hpp"
#include "utils/FormatHelper.hpp"
#include "utils/GameConstants.hpp"
#include "views/IGameUI.hpp"

#include <string>

PetakAksi::PetakAksi(int id, const std::string &name, ActionType type,
                     int amount)
    : Petak(id, name), type(type), amount(amount) {}

ActionType PetakAksi::getType() const { return type; }

int PetakAksi::getAmount() const { return amount; }

#include "core/PaymentManager.hpp"

void PetakAksi::injak(Player &p, IGameUI &ui, int , std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
  switch (type) {
  case ActionType::GO:
    p.addMoney(amount > 0 ? amount : 200);
    break;
  case ActionType::JAIL:
    p.setStatus(PlayerStatus::IN_JAIL);
    p.setPosition(GameConstants::JAIL_POSITION);
    break;

  case ActionType::TAX_FLAT: {
    ui.showMessage("Kamu mendarat di Pajak Penghasilan (PPH)!");

    int cash = p.getMoney();
    int totalPropValue = 0;
    for (auto *prop : p.getOwnedProperties())
      totalPropValue += prop->getTotalWorth();
    int totalKekayaan = cash + totalPropValue;
    int pajak10 = totalKekayaan / 10;

    int pilihan;
    if (p.isComPlayer()) {

      pilihan = (amount <= pajak10) ? 1 : 2;
      ui.showMessage("[COM " + p.getName() + "] Memilih opsi pajak " +
                     std::to_string(pilihan) + " (" +
                     (pilihan == 1 ? "flat " + formatUang(amount)
                                   : "10% = " + formatUang(pajak10)) +
                     ") — otomatis pilih yang lebih murah");
    } else {
      pilihan = ui.promptTaxChoice(amount, pajak10, totalKekayaan);
    }

    if (pilihan == 1) {
      if (board && players) {
          PaymentManager::processPayment(p, nullptr, amount, ui, *board, *players, "");
      } else {
          ui.showMessage("Error: PaymentManager missing board/players context.");
      }
    } else {
      ui.showMessage("Total kekayaan kamu:");
      ui.showMessage("- Uang tunai   : " + formatUang(cash));
      ui.showMessage("- Nilai Properti: " + formatUang(totalPropValue) +
                     " (termasuk bangunan)");
      ui.showMessage("Total          : " + formatUang(totalKekayaan));
      ui.showMessage("Pajak 10%      : " + formatUang(pajak10));

      if (board && players) {
          PaymentManager::processPayment(p, nullptr, pajak10, ui, *board, *players, "");
      } else {
          ui.showMessage("Error: PaymentManager missing board/players context.");
      }
    }
    break;
  }
  case ActionType::TAX_FIXED: {
    ui.showMessage("Kamu mendarat di Pajak Barang Mewah (PBM)!");
    ui.showMessage("Pajak sebesar " + formatUang(amount) +
                   " langsung dipotong.");
    if (board && players) {
        PaymentManager::processPayment(p, nullptr, amount, ui, *board, *players, "");
    } else {
        ui.showMessage("Error: PaymentManager missing board/players context.");
    }
    break;
  }
  case ActionType::FREE_PARKING:
    break;
  case ActionType::FESTIVAL: {
    ui.showMessage("Kamu mendarat di petak Festival!");

    std::vector<Street *> pilihanStreet;
    for (auto *prop : p.getOwnedProperties()) {
      if (auto *s = dynamic_cast<Street *>(prop)) {
        if (!s->getIsMortgaged())
          pilihanStreet.push_back(s);
      }
    }

    if (pilihanStreet.empty()) {
      ui.showMessage(
          "Kamu tidak memiliki properti Street yang dapat dipilih.");
      break;
    }

    if (p.isComPlayer()) {
      Street *best = pilihanStreet[0];
      int bestRent = best->getSewa();
      for (size_t i = 1; i < pilihanStreet.size(); ++i) {
        if (pilihanStreet[i]->getSewa() > bestRent) {
          bestRent = pilihanStreet[i]->getSewa();
          best = pilihanStreet[i];
        }
      }
      ui.showMessage("[COM " + p.getName() + "] Memilih properti festival: " +
                     best->getShortName() + " (" + best->getName() +
                     ") — sewa tertinggi");

      int prevMult = best->getFestivalMultiplier();
      int prevRent = best->getSewa();
      best->activateFestival();
      int newRent = best->getSewa();

      if (prevMult >= 8) {
        ui.showMessage("Efek sudah maksimum (harga sewa sudah digandakan tiga kali)");
        ui.showMessage("Durasi di-reset menjadi: 3 giliran");
      } else if (prevMult > 1) {
        ui.showMessage("Efek diperkuat!");
        ui.showMessage("Sewa sebelumnya: " + formatUang(prevRent));
        ui.showMessage("Sewa sekarang: " + formatUang(newRent));
        ui.showMessage("Durasi di-reset menjadi: 3 giliran");
      } else {
        ui.showMessage("Efek festival aktif!");
        ui.showMessage("Sewa awal: " + formatUang(prevRent));
        ui.showMessage("Sewa sekarang: " + formatUang(newRent));
        ui.showMessage("Durasi: 3 giliran");
      }
      break;
    }

    ui.showMessage("Daftar properti milikmu:");
    for (auto *s : pilihanStreet)
      ui.showMessage("- " + s->getShortName() + " (" + s->getName() + ")");
    while (true) {
      std::string kode = ui.promptInput("Masukkan kode properti: ");

      Street *chosen = nullptr;
      bool milikTapiRailroadUtility = false;
      for (auto *prop : p.getOwnedProperties()) {
        if (prop->getShortName() == kode) {
          chosen = dynamic_cast<Street *>(prop);
          if (!chosen)
            milikTapiRailroadUtility = true;
          break;
        }
      }

      if (milikTapiRailroadUtility) {
        ui.showMessage("-> Properti ini tidak dapat diaktifkan festival!");
        continue;
      }
      if (!chosen) {
        ui.showMessage("-> Kode properti tidak valid atau bukan milikmu!");
        continue;
      }

      int prevMult = chosen->getFestivalMultiplier();
      int prevRent = chosen->getSewa();

      chosen->activateFestival();
      int newRent = chosen->getSewa();

      if (prevMult >= 8) {
        ui.showMessage(
            "Efek sudah maksimum (harga sewa sudah digandakan tiga kali)");
        ui.showMessage("Durasi di-reset menjadi: 3 giliran");
      } else if (prevMult > 1) {
        ui.showMessage("Efek diperkuat!");
        ui.showMessage("Sewa sebelumnya: " + formatUang(prevRent));
        ui.showMessage("Sewa sekarang: " + formatUang(newRent));
        ui.showMessage("Durasi di-reset menjadi: 3 giliran");
      } else {
        ui.showMessage("Efek festival aktif!");
        ui.showMessage("Sewa awal: " + formatUang(prevRent));
        ui.showMessage("Sewa sekarang: " + formatUang(newRent));
        ui.showMessage("Durasi: 3 giliran");
      }
      break;
    }
    break;
  }
  }
}
