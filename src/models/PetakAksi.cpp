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
  }
}
