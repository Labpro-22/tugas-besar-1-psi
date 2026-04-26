#include "models/PetakAksi.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Street.hpp"
#include "models/Utility.hpp"
#include "utils/FormatHelper.hpp"
#include "utils/GameConstants.hpp"
#include "views/IGameUI.hpp"

PetakAksi::PetakAksi(int id, const std::string &name, ActionType type,
                     int amount)
    : Petak(id, name), type(type), amount(amount) {}

ActionType PetakAksi::getType() const { return type; }

int PetakAksi::getAmount() const { return amount; }

void PetakAksi::injak(Player &p, IGameUI &ui, int /*diceRoll*/) {
  switch (type) {
  case ActionType::GO:
    p.addMoney(amount > 0 ? amount : 200);
    break;
  case ActionType::JAIL:
    p.setStatus(PlayerStatus::IN_JAIL);
    p.setPosition(GameConstants::JAIL_POSITION);
    break;
  //FITUR 8: PAJAK
  case ActionType::TAX_FLAT: {
    ui.showMessage("Kamu mendarat di Pajak Penghasilan (PPH)!");

    int cash = p.getMoney();
    int totalPropValue = 0;
    for (auto *prop : p.getOwnedProperties())
      totalPropValue += prop->getTotalWorth();
    int totalKekayaan = cash + totalPropValue;
    int pajak10 = totalKekayaan / 10;

    int pilihan = ui.promptTaxChoice(amount, pajak10, totalKekayaan);

    if (pilihan == 1) {
      if (p.getMoney() >= amount) {
        int before = p.getMoney();
        p.reduceMoney(amount);
        ui.showMessage("Uang kamu: " + formatUang(before) + " -> " +
                       formatUang(p.getMoney()));
      } else {
        ui.showMessage("Kamu tidak mampu membayar pajak flat " +
                       formatUang(amount) + "!");
        ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));
        ui.showMessage("// Alur dilanjutkan ke Kebangkrutan");
        p.reduceMoney(amount);
      }
    } else {
      ui.showMessage("Total kekayaan kamu:");
      ui.showMessage("- Uang tunai   : " + formatUang(cash));
      ui.showMessage("- Nilai Properti: " + formatUang(totalPropValue) +
                     " (termasuk bangunan)");
      ui.showMessage("Total          : " + formatUang(totalKekayaan));
      ui.showMessage("Pajak 10%      : " + formatUang(pajak10));

      if (p.getMoney() >= pajak10) {
        int before = p.getMoney();
        p.reduceMoney(pajak10);
        ui.showMessage("Uang kamu: " + formatUang(before) + " -> " +
                       formatUang(p.getMoney()));
      } else {
        ui.showMessage("Kamu tidak mampu membayar pajak 10% (" +
                       formatUang(pajak10) + ")!");
        ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));
        ui.showMessage("// Alur dilanjutkan ke Kebangkrutan");
        p.reduceMoney(pajak10);
      }
    }
    break;
  }
  case ActionType::TAX_FIXED: {
    ui.showMessage("Kamu mendarat di Pajak Barang Mewah (PBM)!");
    ui.showMessage("Pajak sebesar " + formatUang(amount) +
                   " langsung dipotong.");
    if (p.getMoney() >= amount) {
      int before = p.getMoney();
      p.reduceMoney(amount);
      ui.showMessage("Uang kamu: " + formatUang(before) + " -> " +
                     formatUang(p.getMoney()));
    } else {
      ui.showMessage("Kamu tidak mampu membayar pajak!");
      ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));
      ui.showMessage("// Alur dilanjutkan ke Kebangkrutan");
      p.reduceMoney(amount);
    }
    break;
  }
  case ActionType::FREE_PARKING:
    break;
  case ActionType::FESTIVAL: {
    ui.showMessage("Kamu mendarat di petak Festival!");

    // Kumpulkan Street milik pemain yang tidak tergadai
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

    ui.showMessage("Daftar properti milikmu:");
    for (auto *s : pilihanStreet)
      ui.showMessage("- " + s->getShortName() + " (" + s->getName() + ")");
    while (true) {
      std::string kode = ui.promptInput("Masukkan kode properti: ");

      // Cari di properti milik pemain
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
      int prevRent = chosen->getSewa(); // sewa sebelum aktivasi

      chosen->activateFestival();
      int newRent = chosen->getSewa(); // sewa setelah aktivasi

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
