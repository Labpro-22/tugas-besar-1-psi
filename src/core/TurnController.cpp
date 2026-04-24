#include "core/TurnController.hpp"
#include "core/AuctionManager.hpp"
#include "core/PropertyCommandHandler.hpp"
#include "models/Card.hpp"
#include "models/CardDeck.hpp"
#include "models/Dice.hpp"
#include "models/Petak.hpp"
#include "models/PetakAksi.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Street.hpp"
#include "models/Utility.hpp"
#include "utils/ColorRegistry.hpp"
#include "utils/FormatHelper.hpp"
#include "utils/GameConstants.hpp"
#include "utils/SaveManager.hpp"
#include "views/BoardView.hpp"
#include "views/IGameUI.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

TurnController::TurnController(
    std::vector<std::unique_ptr<Petak>> &board, std::vector<Player> &players,
    int &currentPlayerIndex, int &turnCount, CardDeck &chanceDeck,
    CardDeck &chestDeck, IGameUI &ui, bool &hasDoneAction, Dice &dice,
    std::function<bool(const std::string &)> saveCallback)
    : board(board), players(players), currentPlayerIndex(currentPlayerIndex),
      turnCount(turnCount), chanceDeck(chanceDeck), chestDeck(chestDeck),
      ui(ui), hasDoneAction(hasDoneAction), dice(dice),
      saveCallback(std::move(saveCallback)) {}

void TurnController::executePostRoll(Player &p, int roll1, int roll2,
                                     int &doublesCount, bool &endTurnFlag) {
  int rollTotal = roll1 + roll2;
  ui.showMessage("Hasil: " + std::to_string(roll1) + " + " +
                 std::to_string(roll2) + " = " + std::to_string(rollTotal));

  bool isDouble = (roll1 == roll2);
  if (isDouble) {
    doublesCount++;
    if (doublesCount == 3) {
      ui.showMessage("[!] 3 Kali KEMBAR! Bidak terlempar ke Penjara!");
      p.setPosition(GameConstants::JAIL_POSITION);
      p.setStatus(PlayerStatus::IN_JAIL);
      endTurnFlag = true;
      return;
    }
  }

  if (board.empty()) {
    ui.showMessage("CRITICAL ERROR: Board is not properly initialized.");
    endTurnFlag = true;
    return;
  }

  int previousPosition = p.getPosition();
  p.move(rollTotal, static_cast<int>(board.size()));
  ui.showMessage("Memajukan Bidak " + p.getName() + " sebanyak " +
                 std::to_string(rollTotal) + " petak...");

  if (p.getPosition() < previousPosition) {
    p.addMoney(GameConstants::GO_BONUS);
    ui.showMessage("[!] Melewati GO! Mendapat bonus " +
                   formatUang(GameConstants::GO_BONUS) + ".");
  }

  Petak *currentSpace = board[p.getPosition()].get();
  ui.showMessage("Bidak mendarat di: " + currentSpace->getName() + ".\n");

  currentSpace->injak(p, ui, rollTotal);

  if (auto prop = dynamic_cast<PetakProperti *>(currentSpace)) {
    if (prop->getOwner() == nullptr) {
      std::string kode = prop->getShortName();
      std::string fullColor = ColorRegistry::getFullName(prop->getColorCode());

      if (auto street = dynamic_cast<Street *>(prop)) {
        std::string titleRow =
            "[" + fullColor + "] " + prop->getName() + " (" + kode + ")";
        if (titleRow.length() > 30)
          titleRow = titleRow.substr(0, 27) + "...";

        ui.showMessage("Kamu mendarat di " + prop->getName() + " (" + kode +
                       ")!");
        ui.showMessage("+================================+");

        {
          std::ostringstream oss;
          oss << "|  " << std::left << std::setw(30) << titleRow << "|";
          ui.showMessage(oss.str());
          oss.str("");
          oss << "| Harga Beli    : " << std::left << std::setw(15)
              << formatUang(prop->getHargaBeli()) << "|";
          ui.showMessage(oss.str());
          const auto &rents = street->getRentPrices();
          std::string sewaDasar = !rents.empty() ? formatUang(rents[0]) : "M0";
          oss.str("");
          oss << "| Sewa dasar    : " << std::left << std::setw(15) << sewaDasar
              << "|";
          ui.showMessage(oss.str());
        }

        ui.showMessage("| ...                            |");
        ui.showMessage("+================================+");
        ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));

        std::string choice =
            ui.promptInput("Apakah kamu ingin membeli properti ini seharga " +
                           formatUang(prop->getHargaBeli()) + "? (y/n): ");

        if (choice == "Y" || choice == "y") {
          if (p.getMoney() >= prop->getHargaBeli()) {
            p.reduceMoney(prop->getHargaBeli());
            prop->setOwner(&p);
            p.addProperty(prop);
            ui.showMessage(prop->getName() + " kini menjadi milikmu!");
            ui.showMessage("Uang tersisa: " + formatUang(p.getMoney()));
          } else {
            ui.showMessage("Uang tidak cukup!\nProperti ini akan masuk ke "
                           "sistem lelang...");
            AuctionManager::startAuction(prop, &p, players, ui);
          }
        } else {
          ui.showMessage("Properti ini akan masuk ke sistem lelang...");
          AuctionManager::startAuction(prop, &p, players, ui);
        }
      } else if (dynamic_cast<Railroad *>(prop)) {
        ui.showMessage("Kamu mendarat di " + prop->getName() + " (" + kode +
                       ")!");
        prop->setOwner(&p);
        p.addProperty(prop);
        ui.showMessage("Belum ada yang menginjaknya duluan, stasiun ini kini "
                       "menjadi milikmu!");
      } else if (dynamic_cast<Utility *>(prop)) {
        ui.showMessage("Kamu mendarat di " + prop->getName() + " (" + kode +
                       ")!");
        prop->setOwner(&p);
        p.addProperty(prop);
        ui.showMessage("Belum ada yang menginjaknya duluan, " +
                       prop->getName() + " kini menjadi milikmu!");
      }
    }
  } else if (auto aksi = dynamic_cast<PetakAksi *>(currentSpace)) {
    const std::string &name = aksi->getName();
    if (name == "Chance" || name == "Kesempatan") {
      if (!chanceDeck.isEmpty()) {
        auto drawn = chanceDeck.drawCard();
        ui.showMessage("[CHANCE CARD] " + drawn->getDescription());
        drawn->applyEffect(p);
        chanceDeck.returnCard(std::move(drawn));
      }
    } else if (name == "Community Chest" || name == "Dana Umum") {
      if (!chestDeck.isEmpty()) {
        auto drawn = chestDeck.drawCard();
        ui.showMessage("[COMMUNITY CHEST] " + drawn->getDescription());
        drawn->applyEffect(p);
        chestDeck.returnCard(std::move(drawn));
      }
    }
  }

  if (p.getMoney() < 0) {
    p.setStatus(PlayerStatus::BANKRUPT);
    ui.showMessage("!!! " + p.getName() + " WENT BANKRUPT !!!");
    for (auto prop : p.getOwnedProperties()) {
      prop->lelang();
      AuctionManager::startAuction(prop, &p, players, ui);
    }
    endTurnFlag = true;
    return;
  }

  if (!isDouble)
    endTurnFlag = true;
}

void TurnController::playTurn() {
  Player &p = players[currentPlayerIndex];
  if (p.getStatus() == PlayerStatus::BANKRUPT)
    return;

  hasDoneAction = false;

  ui.showMessage("\n---------------------------------------------");
  ui.showMessage("Turn  | Player: " + p.getName() +
                 " | Saldo: " + formatUang(p.getMoney()));
  ui.showMessage("---------------------------------------------");

  if (p.getStatus() == PlayerStatus::IN_JAIL) {
    ui.showMessage("[!] " + p.getName() + " is in JAIL! Skipping turn...");
    p.setStatus(PlayerStatus::ACTIVE);
    return;
  }

  int doublesCount = 0;
  bool endTurnFlag = false;

  while (!endTurnFlag) {
    std::string command = ui.promptInput("> Masukkan perintah: ");

    if (command == "CETAK_PAPAN") {
      BoardView::printBoard(board, players, turnCount);
      continue;
    }

    std::stringstream ss(command);
    std::string action;
    ss >> action;

    if (action == "LEMPAR_DADU" || command.empty()) {
      hasDoneAction = true;
      ui.showMessage("Mengocok dadu...");
      dice.roll();
      auto rollValues = dice.getLastRoll();
      executePostRoll(p, rollValues.first, rollValues.second, doublesCount,
                      endTurnFlag);
    } else if (action == "CETAK_PAPAN") {
      BoardView::printBoard(board, players, turnCount);
    } else if (action == "CETAK_AKTA") {
      PropertyCommandHandler::cetakAkta(board, ui);
    } else if (action == "CETAK_PROPERTI") {
      std::string targetName;
      std::getline(ss >> std::ws, targetName);
      if (targetName.empty())
        ui.showMessage("Format salah. Gunakan: CETAK_PROPERTI <NAMA_PLAYER>");
      else
        PropertyCommandHandler::cetakProperti(players, ui, targetName);
    } else if (action == "GADAI") {
      hasDoneAction = true;
      PropertyCommandHandler::handleGadai(board, p, ui);
    } else if (action == "TEBUS") {
      hasDoneAction = true;
      PropertyCommandHandler::handleTebus(p, ui);
    } else if (action == "BANGUN") {
      hasDoneAction = true;
      PropertyCommandHandler::handleBangun(board, p, ui);
    } else if (action == "SIMPAN") {
      std::string filename;
      std::getline(ss >> std::ws, filename);
      if (filename.empty()) {
        ui.showMessage("Format salah. Gunakan: SIMPAN <nama_file.txt>");
      } else if (hasDoneAction) {
        ui.showMessage("Kamu sudah melakukan aksi di giliran ini. SIMPAN hanya "
                       "dapat dilakukan di awal giliran.");
      } else if (saveCallback) {
        ui.showMessage("Menyimpan permainan...");
        if (saveCallback(filename))
          ui.showMessage("Permainan berhasil disimpan ke " + filename);
        else
          ui.showMessage("[!] Gagal menyimpan permainan.");
      }
    } else if (action == "ATUR_DADU") {
      hasDoneAction = true;
      int x, y;
      if (ss >> x >> y && x > 0 && y > 0 && x <= 6 && y <= 6) {
        ui.showMessage("Dadu diatur secara manual.");
        executePostRoll(p, x, y, doublesCount, endTurnFlag);
      } else {
        ui.showMessage("[!] Format salah atau angka tidak valid. Gunakan: "
                       "ATUR_DADU X Y (X dan Y > 0)");
      }
    } else if (action == "HELP") {
      ui.showMessage("============= DAFTAR PERINTAH =============");
      ui.showMessage("  CETAK_PAPAN            : Menampilkan peta/board");
      ui.showMessage(
          "  LEMPAR_DADU            : Melempar dadu acak (atau tekan Enter)");
      ui.showMessage(
          "  ATUR_DADU X Y          : Melempar dadu dengan nilai manual");
      ui.showMessage(
          "  CETAK_AKTA             : Melihat info properti berdasarkan kode");
      ui.showMessage(
          "  CETAK_PROPERTI <NAMA>  : Menampilkan properti milik pemain");
      ui.showMessage(
          "  BANGUN                 : Membangun rumah/hotel (menu interaktif)");
      ui.showMessage(
          "  GADAI                  : Menggadaikan properti ke Bank");
      ui.showMessage(
          "  TEBUS                  : Menebus properti yang digadaikan");
      ui.showMessage("  SIMPAN <file>          : Menyimpan state permainan");
      ui.showMessage("  HELP                   : Menampilkan daftar command");
      ui.showMessage("===========================================");
    } else {
      ui.showMessage("[!] Perintah tidak valid.");
    }
  }
}
