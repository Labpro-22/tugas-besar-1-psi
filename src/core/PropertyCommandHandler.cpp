#include "core/PropertyCommandHandler.hpp"
#include "models/Petak.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Street.hpp"
#include "models/Utility.hpp"
#include "utils/ColorRegistry.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

#include <algorithm>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::vector<Street *>
getStreetsInGroup(const std::vector<std::unique_ptr<Petak>> &board,
                  const std::string &colorGroup) {
  std::vector<Street *> result;
  for (const auto &p : board) {
    if (auto s = dynamic_cast<Street *>(p.get())) {
      if (s->getColorGroup() == colorGroup)
        result.push_back(s);
    }
  }
  return result;
}

bool playerHasMonopoly(Player *player,
                       const std::vector<Street *> &groupStreets) {
  if (groupStreets.empty())
    return false;
  for (auto s : groupStreets) {
    if (s->getOwner() != player)
      return false;
  }
  return true;
}

int getMinHouseCount(const std::vector<Street *> &streets) {
  int minCount = 5;
  for (auto s : streets) {
    if (!s->isHotelBuilt())
      minCount = std::min(minCount, s->getHouseCount());
  }
  return minCount;
}

bool allHaveFourHouses(const std::vector<Street *> &streets) {
  for (auto s : streets) {
    if (s->isHotelBuilt() || s->getHouseCount() < 4)
      return false;
  }
  return true;
}

bool canUpgradeAnyToHotel(const std::vector<Street *> &streets) {
  bool hasNonHotel = false;
  for (auto s : streets) {
    if (!s->isHotelBuilt()) {
      hasNonHotel = true;
      if (s->getHouseCount() < 4)
        return false;
    }
  }
  return hasNonHotel;
}
} // namespace

void PropertyCommandHandler::cetakAkta(
    const std::vector<std::unique_ptr<Petak>> &board, IGameUI &ui) {

  std::string kode = ui.promptInput("Masukkan kode petak: ");

  PetakProperti *targetProp = nullptr;
  for (const auto &p : board) {
    if (p && p->getShortName() == kode) {
      targetProp = dynamic_cast<PetakProperti *>(p.get());
      break;
    }
  }

  if (!targetProp) {
    ui.showMessage("Petak \"" + kode +
                   "\" tidak ditemukan atau bukan properti.");
    return;
  }

  std::string fullColor =
      ColorRegistry::getFullName(targetProp->getColorCode());
  std::string titleRow =
      "[" + fullColor + "] " + targetProp->getName() + " (" + kode + ")";
  if (titleRow.length() > 30)
    titleRow = titleRow.substr(0, 27) + "...";

  ui.showMessage("\n+================================+");
  ui.showMessage("|        AKTA KEPEMILIKAN        |");
  {
    std::ostringstream oss;
    oss << "|  " << std::left << std::setw(30) << titleRow << "|";
    ui.showMessage(oss.str());
  }
  ui.showMessage("+================================+");
  {
    std::ostringstream oss;
    oss << "| Harga Beli        : " << std::left << std::setw(11)
        << formatUang(targetProp->getHargaBeli()) << "|";
    ui.showMessage(oss.str());
    oss.str("");
    oss << "| Nilai Gadai       : " << std::left << std::setw(11)
        << formatUang(targetProp->getHargaBeli() / 2) << "|";
    ui.showMessage(oss.str());
  }
  ui.showMessage("+--------------------------------+");

  targetProp->printDeed(ui);

  Player *owner = targetProp->getOwner();
  std::string statusStr;
  if (!owner) {
    statusStr = "TIDAK DIMILIKI";
  } else {
    statusStr = targetProp->getIsMortgaged()
                    ? "MORTGAGED (" + owner->getName() + ")"
                    : "OWNED (" + owner->getName() + ")";
  }

  std::ostringstream oss;
  oss << "| Status : " << std::left << std::setw(22) << statusStr << "|";
  ui.showMessage(oss.str());
  ui.showMessage("+================================+");
}

void PropertyCommandHandler::cetakProperti(const std::vector<Player> &players,
                                           IGameUI &ui,
                                           const std::string &playerName) {

  const Player *targetPlayer = nullptr;
  for (const auto &pl : players) {
    if (pl.getName() == playerName) {
      targetPlayer = &pl;
      break;
    }
  }

  if (!targetPlayer) {
    ui.showMessage("Pemain dengan nama \"" + playerName +
                   "\" tidak ditemukan.");
    return;
  }

  const auto &ownedProps = targetPlayer->getOwnedProperties();
  if (ownedProps.empty()) {
    ui.showMessage(targetPlayer->getName() +
                   " belum memiliki properti apapun.");
    return;
  }

  ui.showMessage("\n> CETAK_PROPERTI " + playerName + "\n");
  ui.showMessage("=== Properti Milik: " + targetPlayer->getName() + " ===\n");

  const std::vector<std::string> order = {
      "COKLAT", "BIRU MUDA", "MERAH MUDA", "ORANGE",   "MERAH",   "KUNING",
      "HIJAU",  "BIRU TUA",  "STASIUN",    "UTILITAS", "ABU-ABU", "LAINNYA"};

  std::map<std::string, std::vector<PetakProperti *>> groups;
  int totalKekayaan = 0;

  for (PetakProperti *prop : ownedProps) {
    std::string groupName = prop->getGroupName();
    int propWealth = prop->getTotalWorth();
    groups[groupName].push_back(prop);
    totalKekayaan += propWealth;
  }

  for (const std::string &gName : order) {
    if (groups.find(gName) == groups.end())
      continue;
    ui.showMessage("[" + gName + "]");
    for (PetakProperti *prop : groups[gName]) {
      std::string fullName =
          prop->getName() + " (" + prop->getShortName() + ")";

      std::string bldStat;
      if (auto s = dynamic_cast<Street *>(prop)) {
        bldStat = s->isHotelBuilt() ? "Hotel"
                  : s->getHouseCount() > 0
                      ? std::to_string(s->getHouseCount()) + " rumah"
                      : "";
      }

      std::string status = prop->getIsMortgaged() ? "MORTGAGED [M]" : "OWNED";

      std::ostringstream oss;
      oss << "- " << std::left << std::setw(26) << fullName << " " << std::left
          << std::setw(9) << bldStat << " " << std::left << std::setw(7)
          << formatUang(prop->getHargaBeli()) << " " << status;
      ui.showMessage(oss.str());
    }
    ui.showMessage("");
  }
  ui.showMessage("Total kekayaan properti: " + formatUang(totalKekayaan));
}

void PropertyCommandHandler::handleBangun(
    const std::vector<std::unique_ptr<Petak>> &board, Player &p, IGameUI &ui) {

  std::vector<std::string> groupOrder;
  std::map<std::string, std::vector<Street *>> monopolyGroups;
  for (const auto &petak : board) {
    if (auto s = dynamic_cast<Street *>(petak.get())) {
      const std::string &cg = s->getColorGroup();
      if (monopolyGroups.count(cg))
        continue;
      auto groupStreets = getStreetsInGroup(board, cg);
      if (playerHasMonopoly(&p, groupStreets)) {
        monopolyGroups[cg] = groupStreets;
        groupOrder.push_back(cg);
      }
    }
  }

  if (monopolyGroups.empty()) {
    ui.showMessage(
        "Tidak ada color group yang memenuhi syarat untuk dibangun.");
    ui.showMessage("Kamu harus memiliki seluruh petak dalam satu color group "
                   "terlebih dahulu.");
    return;
  }

  ui.showMessage("=== Color Group yang Memenuhi Syarat ===");
  for (int gi = 0; gi < (int)groupOrder.size(); gi++) {
    const std::string &cg = groupOrder[gi];
    ui.showMessage(std::to_string(gi + 1) + ". [" +
                   ColorRegistry::getFullName(cg) + "]");
    for (auto s : monopolyGroups[cg]) {
      std::string statusStr =
          s->isHotelBuilt()
              ? "Hotel"
              : std::to_string(s->getHouseCount()) +
                    " rumah (Harga rumah: " + formatUang(s->getHousePrice()) +
                    ")";
      std::ostringstream oss;
      oss << "   - " << std::left << std::setw(30)
          << (s->getName() + " (" + s->getShortName() + ")") << ": "
          << statusStr;
      ui.showMessage(oss.str());
    }
  }

  ui.showMessage("\nUang kamu saat ini : " + formatUang(p.getMoney()));
  std::string input =
      ui.promptInput("Pilih nomor color group (0 untuk batal): ");
  int groupChoice = 0;
  try {
    groupChoice = std::stoi(input);
  } catch (...) {
  }
  if (groupChoice <= 0 || groupChoice > (int)groupOrder.size()) {
    ui.showMessage("Dibatalkan.");
    return;
  }

  const std::string &selectedCode = groupOrder[groupChoice - 1];
  std::string selectedFullName = ColorRegistry::getFullName(selectedCode);
  auto &groupStreets = monopolyGroups[selectedCode];

  bool hotelReady = canUpgradeAnyToHotel(groupStreets);
  bool allFour = allHaveFourHouses(groupStreets);
  int minCount = getMinHouseCount(groupStreets);

  ui.showMessage("\nColor group [" + selectedFullName + "]:");
  for (int si = 0; si < (int)groupStreets.size(); si++) {
    auto s = groupStreets[si];
    std::string nameKode = s->getName() + " (" + s->getShortName() + ")";
    std::string statusStr, annotation;

    if (s->isHotelBuilt()) {
      statusStr = "Hotel";
      annotation = "<- sudah maksimal, tidak dapat dibangun";
    } else if (s->getHouseCount() == 4 && hotelReady) {
      statusStr = "4 rumah";
      annotation = "<- siap upgrade ke hotel";
    } else if (s->getHouseCount() == 4) {
      statusStr = "4 rumah";
      annotation = "(menunggu petak lain)";
    } else if (s->getHouseCount() == minCount) {
      statusStr = std::to_string(s->getHouseCount()) + " rumah";
      annotation = "<- dapat dibangun";
    } else {
      statusStr = std::to_string(s->getHouseCount()) + " rumah";
      annotation = "(menunggu petak lain)";
    }
    std::ostringstream oss;
    oss << "- " << std::left << std::setw(30) << nameKode << ": " << std::left
        << std::setw(8) << statusStr << " " << annotation;
    ui.showMessage(oss.str());
  }

  if (allFour) {
    ui.showMessage("\nSeluruh color group [" + selectedFullName +
                   "] sudah memiliki 4 rumah. Siap di-upgrade ke hotel!");
  }

  input = ui.promptInput("Pilih petak (0 untuk batal): ");
  int streetChoice = 0;
  try {
    streetChoice = std::stoi(input);
  } catch (...) {
  }
  if (streetChoice <= 0 || streetChoice > (int)groupStreets.size()) {
    ui.showMessage("Dibatalkan.");
    return;
  }

  Street *target = groupStreets[streetChoice - 1];

  if (target->isHotelBuilt()) {
    ui.showMessage("[!] " + target->getName() + " sudah memiliki hotel.");
    return;
  }

  if (target->getHouseCount() == 4) {
    if (!hotelReady) {
      ui.showMessage("[!] Semua petak di grup harus memiliki 4 rumah sebelum "
                     "bisa upgrade ke hotel.");
      return;
    }
    int cost = target->getHotelPrice();
    input = ui.promptInput("Upgrade ke hotel? Biaya: " + formatUang(cost) +
                           " (y/n): ");
    if (input != "y" && input != "Y") {
      ui.showMessage("Dibatalkan.");
      return;
    }
    if (p.getMoney() < cost) {
      ui.showMessage("[!] Uang tidak cukup untuk hotel (butuh " +
                     formatUang(cost) + ").");
      return;
    }
    p.reduceMoney(cost);
    target->upgradeToHotel();
    ui.showMessage(target->getName() + " di-upgrade ke Hotel!");
    ui.showMessage("Uang tersisa: " + formatUang(p.getMoney()));
    return;
  }

  if (target->getHouseCount() > minCount) {
    ui.showMessage("[!] Pembangunan harus merata. Bangun di petak lain yang "
                   "masih memiliki " +
                   std::to_string(minCount) + " rumah.");
    return;
  }
  int cost = target->getHousePrice();
  if (p.getMoney() < cost) {
    ui.showMessage("[!] Uang tidak cukup untuk rumah (butuh " +
                   formatUang(cost) + ").");
    return;
  }
  p.reduceMoney(cost);
  target->buildHouse();
  ui.showMessage("Kamu membangun 1 rumah di " + target->getName() +
                 ". Biaya: " + formatUang(cost));
  ui.showMessage("Uang tersisa: " + formatUang(p.getMoney()));

  for (auto s : groupStreets) {
    std::string statusStr = s->isHotelBuilt()
                                ? "Hotel"
                                : std::to_string(s->getHouseCount()) + " rumah";
    std::ostringstream oss;
    oss << "- " << std::left << std::setw(30)
        << (s->getName() + " (" + s->getShortName() + ")") << ": " << statusStr;
    ui.showMessage(oss.str());
  }
}

void PropertyCommandHandler::handleGadai(
    const std::vector<std::unique_ptr<Petak>> &board, Player &p, IGameUI &ui) {

  std::vector<PetakProperti *> gadaiable;
  for (auto prop : p.getOwnedProperties()) {
    if (!prop->getIsMortgaged())
      gadaiable.push_back(prop);
  }

  if (gadaiable.empty()) {
    ui.showMessage("Kamu tidak memiliki properti yang dapat digadaikan.");
    return;
  }

  ui.showMessage("=== Properti yang Dapat Digadaikan ===");
  for (size_t i = 0; i < gadaiable.size(); i++) {
    PetakProperti *prop = gadaiable[i];
    std::string groupName = prop->getGroupName();
    int nilaiGadai = prop->getHargaBeli() / 2;
    ui.showMessage(std::to_string(i + 1) + ". " + prop->getName() + " (" +
                   prop->getShortName() + ") [" + groupName +
                   "] Nilai Gadai: " + formatUang(nilaiGadai));
  }

  std::string input = ui.promptInput("Pilih nomor properti (0 untuk batal): ");
  int choice = 0;
  try {
    choice = std::stoi(input);
  } catch (...) {
  }
  if (choice <= 0 || choice > (int)gadaiable.size())
    return;

  PetakProperti *target = gadaiable[choice - 1];

  if (auto s = dynamic_cast<Street *>(target)) {
    auto groupStreets = getStreetsInGroup(board, s->getColorGroup());
    bool hasBuildings = false;
    int totalSellValue = 0;
    for (auto st : groupStreets) {
      if (st->getHouseCount() > 0 || st->isHotelBuilt()) {
        hasBuildings = true;
        totalSellValue +=
            st->isHotelBuilt()
                ? (st->getHotelPrice() / 2) + (st->getHousePrice() / 2) * 4
                : (st->getHousePrice() / 2) * st->getHouseCount();
      }
    }

    if (hasBuildings) {
      ui.showMessage("Ada bangunan di color group " +
                     ColorRegistry::getFullName(s->getColorGroup()) + ".");
      ui.showMessage("Kamu wajib menjual semua bangunan di color group "
                     "tersebut (seharga " +
                     formatUang(totalSellValue) + ") untuk menggadai " +
                     s->getName() + ".");
      input = ui.promptInput("Lanjutkan? (y/n): ");
      if (input != "y" && input != "Y") {
        ui.showMessage("Gadai dibatalkan.");
        return;
      }
      for (auto st : groupStreets)
        st->demolishAllBuildings();
      p.addMoney(totalSellValue);
      ui.showMessage("Semua bangunan di color group terjual. Kamu menerima " +
                     formatUang(totalSellValue) + ".");
    }
  }

  target->gadai();
  int nilaiGadai = target->getHargaBeli() / 2;
  p.addMoney(nilaiGadai);
  ui.showMessage(target->getName() + " berhasil digadaikan.");
  ui.showMessage("Kamu menerima " + formatUang(nilaiGadai) + " dari Bank.");
  ui.showMessage("Uang kamu sekarang: " + formatUang(p.getMoney()));
  ui.showMessage(
      "Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");
}

void PropertyCommandHandler::handleTebus(Player &p, IGameUI &ui) {
  std::vector<PetakProperti *> tebusable;
  for (auto prop : p.getOwnedProperties()) {
    if (prop->getIsMortgaged())
      tebusable.push_back(prop);
  }

  if (tebusable.empty()) {
    ui.showMessage("Kamu tidak memiliki properti yang sedang digadaikan.");
    return;
  }

  ui.showMessage("=== Properti yang Sedang Digadaikan ===");
  for (size_t i = 0; i < tebusable.size(); i++) {
    PetakProperti *prop = tebusable[i];
    std::string groupName = prop->getGroupName(); // virtual — no cast
    int hargaTebus = prop->getHargaBeli();
    ui.showMessage(std::to_string(i + 1) + ". " + prop->getName() + " (" +
                   prop->getShortName() + ") [" + groupName +
                   "] [M] Harga Tebus: " + formatUang(hargaTebus));
  }

  ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));
  std::string input = ui.promptInput("Pilih nomor properti (0 untuk batal): ");
  int choice = 0;
  try {
    choice = std::stoi(input);
  } catch (...) {
  }
  if (choice <= 0 || choice > (int)tebusable.size())
    return;

  PetakProperti *target = tebusable[choice - 1];
  int hargaTebus = target->getHargaBeli();

  try {
    if (p.getMoney() < hargaTebus)
      throw std::invalid_argument("Uang tidak cukup untuk menebus properti!");
    p.reduceMoney(hargaTebus);
    target->tebus();
    ui.showMessage(target->getName() + " berhasil ditebus!");
    ui.showMessage("Kamu membayar " + formatUang(hargaTebus) + " ke Bank.");
    ui.showMessage("Uang kamu sekarang: " + formatUang(p.getMoney()));
  } catch (const std::invalid_argument &e) {
    ui.showMessage("[!] " + std::string(e.what()));
  }
}
