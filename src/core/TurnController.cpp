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
#include <sstream>


TurnController::TurnController(
    std::vector<std::unique_ptr<Petak>> &board_,
    std::vector<Player>                 &players_,
    int      &currentPlayerIndex_,
    int      &turnCount_,
    CardDeck &chanceDeck_,
    CardDeck &chestDeck_,
    IGameUI  &ui_,
    bool     &hasDoneAction_,
    Dice     &dice_,
    std::function<bool(const std::string &)>  cbSave_,
    std::function<void(const std::string &)>  cbLog_,
    std::function<void *()>     cbDraw_,
    std::function<void(void *)> cbDiscard_,
    const std::vector<std::string>           &transactionLog_)
    : board(board_), players(players_),
      currentPlayerIndex(currentPlayerIndex_), turnCount(turnCount_),
      chanceDeck(chanceDeck_), chestDeck(chestDeck_), ui(ui_),
      hasDoneAction(hasDoneAction_), dice(dice_),
      transactionLog(transactionLog_),
      cbSave(std::move(cbSave_)),
      cbLog(std::move(cbLog_)),
      cbDraw(std::move(cbDraw_)),
      cbDiscard(std::move(cbDiscard_)) {}

void TurnController::addLog(const std::string &jenis,
                            const std::string &detail,
                            const std::string &username) {
  if (cbLog)
    cbLog(std::to_string(turnCount) + " " + username + " " + jenis +
          " " + detail);
}


void TurnController::printLog(int n) const {
  if (transactionLog.empty()) {
    ui.showMessage("Log transaksi kosong.");
    return;
  }
  int total = static_cast<int>(transactionLog.size());
  int start = (n > 0 && n < total) ? total - n : 0;

  if (n > 0)
    ui.showMessage("=== Log Transaksi (" + std::to_string(n) +
                   " Terakhir) ===");
  else
    ui.showMessage("=== Log Transaksi Penuh ===");

  for (int i = start; i < total; ++i) {
    std::istringstream ss(transactionLog[i]);
    std::string turnNum, user, jenis, detail;
    ss >> turnNum >> user >> jenis;
    std::getline(ss >> std::ws, detail);
    ui.showMessage("[Turn " + turnNum + "] " + user + " | " +
                   jenis + " | " + detail);
  }
}


void TurnController::handleDropCard(Player &p) {
  const auto &hand = p.getHand();
  int sz = static_cast<int>(hand.size());

  ui.showMessage("PERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal 3)!\nKamu diwajibkan membuang 1 kartu.\n");
  ui.showMessage("Daftar Kartu Kemampuan Anda:");
  for (int i = 0; i < sz; ++i)
    ui.showMessage(std::to_string(i + 1) + ". " +
                   hand[i]->getTypeName() + " - " +
                   hand[i]->getDescription());

  while (true) {
    std::string inp = ui.promptInput(
        "\nPilih nomor kartu yang ingin dibuang (1-" +
        std::to_string(sz) + "): ");
    int choice = 0;
    try { choice = std::stoi(inp); } catch (...) {}

    if (choice < 1 || choice > sz) {
      ui.showMessage("[!] Nomor tidak valid.");
      continue;
    }
    int idx = choice - 1;
    std::string nama = hand[idx]->getTypeName();

    // Buat SpecialCard baru untuk discard (raw pointer lewat void*)
    SpecialCard *rawDiscard = new SpecialCard(
        hand[idx]->getType(), hand[idx]->getValue(),
        hand[idx]->getDuration());
    p.removeSpecialCard(idx);
    if (cbDiscard)
      cbDiscard(static_cast<void *>(rawDiscard));
    else
      delete rawDiscard;

    ui.showMessage("\n" + nama + " telah dibuang. Sekarang kamu memiliki " +
                   std::to_string(p.getHandSize()) + " kartu di\ntangan.");
    addLog("DROP_KARTU", "Buang " + nama, p.getName());
    break;
  }
}


void TurnController::handleTurnStart(Player &p) {
  // Reset shield and discount
  p.setShieldActive(false);
  p.setDiscount(0);

  // Kurangi durasi festival tiap Street milik pemain
  for (auto *prop : p.getOwnedProperties()) {
    if (auto *s = dynamic_cast<Street *>(prop)) {
      if (s->getFestivalDuration() > 0)
        s->decrementFestival();
    }
  }

  // Bagikan 1 kartu kemampuan acak
  if (!cbDraw) return;
  void *rawCard = cbDraw();
  if (!rawCard) return;

  auto *card = static_cast<SpecialCard *>(rawCard);
  std::string cardName = card->getTypeName();
  ui.showMessage("\nKamu mendapatkan 1 kartu acak baru!");
  ui.showMessage("Kartu yang didapat: " + cardName + ".");
  addLog("DAPAT_KARTU", cardName, p.getName());

  p.addSpecialCard(std::unique_ptr<SpecialCard>(card));

  // 3. Jika tangan > 3, maka DROP
  if (p.getHandSize() > 3)
    handleDropCard(p);
}


void TurnController::executePostRoll(Player &p, int roll1, int roll2,
                                     int &doublesCount, bool &endTurnFlag) {
  int rollTotal = roll1 + roll2;
  ui.showMessage("Hasil: " + std::to_string(roll1) + " + " +
                 std::to_string(roll2) + " = " + std::to_string(rollTotal));
  addLog("DADU",
         "Lempar: " + std::to_string(roll1) + "+" +
             std::to_string(roll2) + "=" + std::to_string(rollTotal),
         p.getName());

  bool isDouble = (roll1 == roll2);
  if (isDouble) {
    ++doublesCount;
    if (doublesCount == 3) {
      ui.showMessage("[!] 3 Kali KEMBAR! Bidak terlempar ke Penjara!");
      p.setPosition(GameConstants::JAIL_POSITION);
      p.setStatus(PlayerStatus::IN_JAIL);
      addLog("PENJARA", "3x double → masuk penjara", p.getName());
      endTurnFlag = true;
      return;
    }
  }

  if (board.empty()) {
    ui.showMessage("CRITICAL ERROR: Board kosong.");
    endTurnFlag = true;
    return;
  }

  int prevPos = p.getPosition();
  p.move(rollTotal, static_cast<int>(board.size()));
  ui.showMessage("Memajukan Bidak " + p.getName() + " sebanyak " +
                 std::to_string(rollTotal) + " petak...");

  if (p.getPosition() < prevPos) {
    p.addMoney(GameConstants::GO_BONUS);
    ui.showMessage("[!] Melewati GO! Mendapat bonus " +
                   formatUang(GameConstants::GO_BONUS) + ".");
    addLog("GO", "Melewati GO, dapat " + formatUang(GameConstants::GO_BONUS),
           p.getName());
  }

  Petak *cur = board[p.getPosition()].get();
  ui.showMessage("Bidak mendarat di: " + cur->getName() + ".\n");
  cur->injak(p, ui, rollTotal, &board, &players);

  if (auto *prop = dynamic_cast<PetakProperti *>(cur)) {
    std::string kode = prop->getShortName();

    if (prop->getOwner() == nullptr) {
      std::string fullColor = ColorRegistry::getFullName(prop->getColorCode());

      if (auto *street = dynamic_cast<Street *>(prop)) {
        std::string titleRow =
            "[" + fullColor + "] " + prop->getName() + " (" + kode + ")";
        if (titleRow.length() > 30)
          titleRow = titleRow.substr(0, 27) + "...";

        ui.showMessage("Kamu mendarat di " + prop->getName() +
                       " (" + kode + ")!");
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
          std::string sewaDasar = rents.empty() ? "M0" : formatUang(rents[0]);
          oss.str("");
          oss << "| Sewa dasar    : " << std::left << std::setw(15)
              << sewaDasar << "|";
          ui.showMessage(oss.str());
        }
        ui.showMessage("| ...                            |");
        ui.showMessage("+================================+");
        ui.showMessage("Uang kamu saat ini: " + formatUang(p.getMoney()));

        std::string ch = ui.promptInput(
            "Apakah kamu ingin membeli properti ini seharga " +
            formatUang(prop->getHargaBeli()) + "? (y/n): ");

        if (ch == "Y" || ch == "y") {
          if (p.getMoney() >= prop->getHargaBeli()) {
            p.reduceMoney(prop->getHargaBeli());
            prop->setOwner(&p);
            p.addProperty(prop);
            ui.showMessage(prop->getName() + " kini menjadi milikmu!");
            ui.showMessage("Uang tersisa: " + formatUang(p.getMoney()));
            addLog("BELI",
                   "Beli " + prop->getName() + " (" + kode + ") seharga " +
                       formatUang(prop->getHargaBeli()),
                   p.getName());
          } else {
            ui.showMessage("Uang tidak cukup!\n"
                           "Properti ini akan masuk ke sistem lelang...");
            AuctionManager::startAuction(prop, &p, players, ui);
          }
        } else {
          ui.showMessage("Properti ini akan masuk ke sistem lelang...");
          AuctionManager::startAuction(prop, &p, players, ui);
        }

      } else if (dynamic_cast<Railroad *>(prop)) {
        ui.showMessage("Kamu mendarat di " + prop->getName() +
                       " (" + kode + ")!");
        prop->setOwner(&p);
        p.addProperty(prop);
        ui.showMessage("Belum ada yang menginjaknya duluan, "
                       "stasiun ini kini menjadi milikmu!");
        addLog("RAILROAD",
               prop->getName() + " (" + kode + ") kini milik " +
                   p.getName() + " (otomatis)",
               p.getName());

      } else if (dynamic_cast<Utility *>(prop)) {
        ui.showMessage("Kamu mendarat di " + prop->getName() +
                       " (" + kode + ")!");
        prop->setOwner(&p);
        p.addProperty(prop);
        ui.showMessage("Belum ada yang menginjaknya duluan, " +
                       prop->getName() + " kini menjadi milikmu!");
        addLog("UTILITY",
               prop->getName() + " (" + kode + ") kini milik " +
                   p.getName() + " (otomatis)",
               p.getName());
      }

    } else if (prop->getOwner() != &p && !prop->getIsMortgaged()) {
      int rent = prop->getSewa(rollTotal);
      addLog("SEWA",
             "Bayar " + formatUang(rent) + " ke " +
                 prop->getOwner()->getName() + " (" + kode + ")",
             p.getName());
    }

  } else if (auto *aksi = dynamic_cast<PetakAksi *>(cur)) {
    const std::string &aname = aksi->getName();

    if (aname == "Kesempatan" || aname == "Chance") {
      ui.showMessage("Kamu mendarat di Petak Kesempatan!");
      ui.showMessage("Mengambil kartu...");
      if (!chanceDeck.isEmpty()) {
        auto drawn = chanceDeck.drawCard();
        ui.showMessage("Kartu: \"" + drawn->getDescription() + "\"");
        addLog("KARTU", "Kesempatan: " + drawn->getDescription(), p.getName());
        drawn->applyEffect(p, ui, &board, &players);
        chanceDeck.returnCard(std::move(drawn));
      }
    } else if (aname == "Dana_Umum" || aname == "Dana Umum" ||
               aname == "Community Chest") {
      ui.showMessage("Kamu mendarat di Petak Dana Umum!");
      ui.showMessage("Mengambil kartu...");
      if (!chestDeck.isEmpty()) {
        auto drawn = chestDeck.drawCard();
        ui.showMessage("Kartu: \"" + drawn->getDescription() + "\"");
        addLog("KARTU", "Dana Umum: " + drawn->getDescription(), p.getName());
        drawn->applyEffect(p, ui, &board, &players);
        chestDeck.returnCard(std::move(drawn));
      }
    } else if (aksi->getType() == ActionType::FESTIVAL) {
      addLog("FESTIVAL", "Mendarat di Festival", p.getName());
    }
  }

  if (p.getStatus() == PlayerStatus::BANKRUPT) {
    addLog("BANGKRUT", "", p.getName());
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
  handleTurnStart(p);

  ui.showMessage("\n---------------------------------------------");
  ui.showMessage("Turn  | Player: " + p.getName() +
                 " | Saldo: " + formatUang(p.getMoney()));
  ui.showMessage("---------------------------------------------");

  if (p.getStatus() == PlayerStatus::IN_JAIL) {
    ui.showMessage("[!] " + p.getName() + " is in JAIL! Skipping turn...");
    p.setStatus(PlayerStatus::ACTIVE);
    return;
  }

  int  doublesCount = 0;
  bool endTurnFlag  = false;
  bool hasUsedAbility = false;

  while (!endTurnFlag) {
    std::string command = ui.promptInput("> Masukkan perintah: ");

    std::stringstream ss(command);
    std::string action;
    ss >> action;

    if (action == "GUNAKAN_KEMAMPUAN") {
      if (hasDoneAction) {
        ui.showMessage("Kartu kemampuan hanya bisa digunakan SEBELUM melempar dadu.");
      } else if (hasUsedAbility) {
        ui.showMessage("Kamu sudah menggunakan kartu kemampuan pada giliran ini!\nPenggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.");
      } else {
        const auto &hand = p.getHand();
        if (hand.empty()) {
          ui.showMessage("Kamu tidak memiliki kartu kemampuan spesial.");
        } else {
          ui.showMessage("Daftar Kartu Kemampuan Spesial Anda:");
          for (size_t i = 0; i < hand.size(); ++i) {
            ui.showMessage(std::to_string(i + 1) + ". " + hand[i]->getTypeName() + " - " + hand[i]->getDescription());
          }
          ui.showMessage("0. Batal");
          
          std::string choiceStr = ui.promptInput("Pilih kartu yang ingin digunakan (0-" + std::to_string(hand.size()) + "): ");
          int choice = -1;
          try { choice = std::stoi(choiceStr); } catch (...) {}
          
          if (choice == 0) {
            // Cancelled
          } else if (choice > 0 && choice <= (int)hand.size()) {
            int idx = choice - 1;
            ui.showMessage(hand[idx]->getTypeName() + " diaktifkan! " + hand[idx]->getDescription());
            addLog("KEMAMPUAN", "Gunakan " + hand[idx]->getTypeName(), p.getName());
            
            hand[idx]->applyEffect(p, ui, &board, &players);
            
            SpecialCard *rawDiscard = new SpecialCard(hand[idx]->getType(), hand[idx]->getValue(), hand[idx]->getDuration());
            p.removeSpecialCard(idx);
            if (cbDiscard) {
              cbDiscard(static_cast<void *>(rawDiscard));
            } else {
              delete rawDiscard;
            }
            
            hasUsedAbility = true;
          } else {
            ui.showMessage("Pilihan tidak valid.");
          }
        }
      }
    } else if (action == "CETAK_PAPAN") {
      BoardView::printBoard(board, players, turnCount);

    } else if (action == "LEMPAR_DADU" || action.empty()) {
      hasDoneAction = true;
      ui.showMessage("Mengocok dadu...");
      dice.roll();
      auto rv = dice.getLastRoll();
      executePostRoll(p, rv.first, rv.second, doublesCount, endTurnFlag);

    } else if (action == "ATUR_DADU") {
      hasDoneAction = true;
      int x = 0, y = 0;
      ss >> x >> y;
      if (x >= 1 && x <= 6 && y >= 1 && y <= 6) {
        ui.showMessage("Dadu diatur secara manual.");
        executePostRoll(p, x, y, doublesCount, endTurnFlag);
      } else {
        ui.showMessage("[!] Format: ATUR_DADU X Y (1-6)");
        hasDoneAction = false;
      }

    } else if (action == "CETAK_AKTA") {
      PropertyCommandHandler::cetakAkta(board, ui);

    } else if (action == "CETAK_PROPERTI") {
      std::string targetName;
      std::getline(ss >> std::ws, targetName);
      if (targetName.empty())
        ui.showMessage("Format: CETAK_PROPERTI <NAMA_PLAYER>");
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
        ui.showMessage("Format: SIMPAN <nama_file.txt>");
      } else if (hasDoneAction) {
        ui.showMessage("SIMPAN hanya bisa di awal giliran (sebelum lempar dadu).");
      } else if (cbSave) {
        ui.showMessage("Menyimpan permainan...");
        if (cbSave(filename))
          ui.showMessage("Permainan berhasil disimpan ke " + filename);
        else
          ui.showMessage("[!] Gagal menyimpan permainan.");
      }

    } else if (action == "CETAK_LOG") {
      int n = 0;
      ss >> n; // 0 = semua
      printLog(n);

    } else if (action == "HELP") {
      ui.showMessage("============= DAFTAR PERINTAH =============");
      ui.showMessage("  LEMPAR_DADU            : Lempar dadu acak");
      ui.showMessage("  ATUR_DADU X Y          : Lempar dadu manual");
      ui.showMessage("  CETAK_PAPAN            : Tampilkan papan");
      ui.showMessage("  CETAK_AKTA             : Info properti");
      ui.showMessage("  CETAK_PROPERTI <NAMA>  : Properti milik pemain");
      ui.showMessage("  BANGUN                 : Bangun rumah/hotel");
      ui.showMessage("  GADAI                  : Gadai properti ke Bank");
      ui.showMessage("  TEBUS                  : Tebus properti");
      ui.showMessage("  GUNAKAN_KEMAMPUAN      : Gunakan kartu spesial");
      ui.showMessage("  SIMPAN <file>          : Simpan permainan");
      ui.showMessage("  CETAK_LOG [n]          : Cetak log (n terakhir)");
      ui.showMessage("===========================================");

    } else {
      ui.showMessage("[!] Perintah tidak dikenal: " + action);
    }
  }
}