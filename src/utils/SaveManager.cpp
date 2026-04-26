#include "utils/SaveManager.hpp"
#include "core/GameManager.hpp"
#include "models/Card.hpp"
#include "models/Petak.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Street.hpp"
#include "models/Utility.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>


static std::string posCodeOf(int pos,
                             const std::vector<std::unique_ptr<Petak>> &board) {
  if (pos >= 0 && pos < static_cast<int>(board.size()) && board[pos])
    return board[pos]->getShortName();
  return "GO";
}

static int posIndexOf(const std::string &code,
                      const std::vector<std::unique_ptr<Petak>> &board) {
  for (int i = 0; i < static_cast<int>(board.size()); ++i)
    if (board[i] && board[i]->getShortName() == code)
      return i;
  return 0;
}

bool SaveManager::saveGame(const GameManager &gm,
                           const std::string &filename) {
  if (std::filesystem::exists(filename)) {
    std::cout << "File \"" << filename << "\" sudah ada. Timpa file lama? (y/n): ";
    std::string choice;
    std::getline(std::cin, choice);
    if (choice != "y" && choice != "Y") {
      std::cout << "Penyimpanan dibatalkan.\n";
      return false;
    }
  }

  std::ofstream out(filename);
  if (!out) {
    std::cout << "Gagal membuat file " << filename << "\n";
    return false;
  }

  const auto &players = gm.getPlayers();
  const auto &board   = gm.getBoard();
  int numPlayers = static_cast<int>(players.size());

  
  out << gm.getTurnCount() << " " << gm.getMaxTurn() << "\n";
  
  out << numPlayers << "\n";

  
  for (const auto &p : players) {
    std::string statusStr;
    switch (p.getStatus()) {
    case PlayerStatus::ACTIVE:   statusStr = "ACTIVE";   break;
    case PlayerStatus::IN_JAIL:  statusStr = "JAILED";   break;
    case PlayerStatus::BANKRUPT: statusStr = "BANKRUPT"; break;
    }
    out << p.getName() << " " << p.getMoney() << " "
        << posCodeOf(p.getPosition(), board) << " " << statusStr << "\n";

    const auto &hand = p.getHand();
    out << hand.size() << "\n";
    for (const auto &card : hand) {
      out << card->getTypeName();
      
      if (card->getType() == SpecialCardType::MOVE ||
          card->getType() == SpecialCardType::DISCOUNT)
        out << " " << card->getValue();
      else
        out << " -";
      
      if (card->getType() == SpecialCardType::DISCOUNT)
        out << " " << card->getDuration();
      else
        out << " -";
      out << "\n";
    }
  }

  for (int i = 0; i < numPlayers; ++i) {
    out << players[i].getName();
    if (i < numPlayers - 1) out << " ";
  }
  out << "\n";
  out << players[gm.getCurrentPlayerIndex()].getName() << "\n";

  
  std::vector<const PetakProperti *> allProps;
  for (const auto &petak : board) {
    if (auto *pp = dynamic_cast<const PetakProperti *>(petak.get()))
      allProps.push_back(pp);
  }
  out << allProps.size() << "\n";
  for (const auto *pp : allProps) {
    std::string jenis, statusStr, pemilik;
    if (dynamic_cast<const Street *>(pp))        jenis = "street";
    else if (dynamic_cast<const Railroad *>(pp)) jenis = "railroad";
    else                                          jenis = "utility";

    if (!pp->getOwner())                                    statusStr = "BANK";
    else if (pp->getIsMortgaged())                          statusStr = "MORTGAGED";
    else                                                    statusStr = "OWNED";
    pemilik = pp->getOwner() ? pp->getOwner()->getName() : "BANK";

    int fmult = 1, fdur = 0, nbuild = 0;
    std::string nbuildStr = "0";
    if (auto *s = dynamic_cast<const Street *>(pp)) {
      fmult     = s->getFestivalMultiplier();
      fdur      = s->getFestivalDuration();
      if (s->isHotelBuilt())       nbuildStr = "H";
      else                         nbuildStr = std::to_string(s->getHouseCount());
    }

    out << pp->getShortName() << " " << jenis << " " << pemilik << " "
        << statusStr << " " << fmult << " " << fdur << " "
        << nbuildStr << "\n";
  }

  const auto &log = gm.getLog();
  out << log.size() << "\n";
  for (const auto &entry : log)
    out << entry << "\n";

  out.close();
  return true;
}


bool SaveManager::loadGame(GameManager &gm, const std::string &filename) {
  std::ifstream in(filename);
  if (!in) {
    std::cout << "File \"" << filename << "\" tidak ditemukan.\n";
    return false;
  }

  auto &players = gm.getPlayersRef();
  auto &board   = gm.getBoardRef();
  players.clear();

  try {
    int turnCount = 0, maxTurn = 0;
    if (!(in >> turnCount >> maxTurn))
      throw std::runtime_error("Format tidak valid (baris 1).");
    gm.setTurnCount(turnCount);
    gm.setMaxTurn(maxTurn);

    int numPlayers = 0;
    if (!(in >> numPlayers) || numPlayers < 2 || numPlayers > 4)
      throw std::runtime_error("Jumlah pemain tidak valid.");
    in.ignore();

    for (int i = 0; i < numPlayers; ++i) {
      std::string uname, posCode, statusStr;
      int money = 0;
      if (!(in >> uname >> money >> posCode >> statusStr))
        throw std::runtime_error("Format pemain tidak valid.");
      in.ignore();

      Player p(i + 1, uname, money);
      p.setPosition(posIndexOf(posCode, board));

      if (statusStr == "JAILED")   p.setStatus(PlayerStatus::IN_JAIL);
      else if (statusStr == "BANKRUPT") p.setStatus(PlayerStatus::BANKRUPT);

      int numCards = 0;
      if (!(in >> numCards)) throw std::runtime_error("Jumlah kartu tidak valid.");
      in.ignore();

      for (int c = 0; c < numCards; ++c) {
        std::string cardType, valStr, durStr;
        if (!(in >> cardType >> valStr >> durStr))
          throw std::runtime_error("Format kartu tidak valid.");
        in.ignore();

        SpecialCardType sct = SpecialCardType::SHIELD;
        if      (cardType == "MoveCard")       sct = SpecialCardType::MOVE;
        else if (cardType == "DiscountCard")   sct = SpecialCardType::DISCOUNT;
        else if (cardType == "ShieldCard")     sct = SpecialCardType::SHIELD;
        else if (cardType == "TeleportCard")   sct = SpecialCardType::TELEPORT;
        else if (cardType == "LassoCard")      sct = SpecialCardType::LASSO;
        else if (cardType == "DemolitionCard") sct = SpecialCardType::DEMOLITION;

        int val = (valStr != "-") ? std::stoi(valStr) : 0;
        int dur = (durStr != "-") ? std::stoi(durStr) : 0;
        p.addSpecialCard(std::make_unique<SpecialCard>(sct, val, dur));
      }

      players.push_back(std::move(p));
    }

    std::string orderLine;
    if (!std::getline(in, orderLine))
      throw std::runtime_error("Baris urutan giliran tidak ditemukan.");

    
    std::string currentName;
    if (!std::getline(in, currentName))
      throw std::runtime_error("Baris pemain aktif tidak ditemukan.");
    while (!currentName.empty() && (currentName.back() == '\r' ||
                                    currentName.back() == '\n' ||
                                    currentName.back() == ' '))
      currentName.pop_back();

    int currentIdx = 0;
    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
      if (players[i].getName() == currentName) {
        currentIdx = i;
        break;
      }
    }
    gm.setCurrentPlayerIndex(currentIdx);

    int numProps = 0;
    if (!(in >> numProps)) throw std::runtime_error("Jumlah properti tidak valid.");
    in.ignore();

    for (int i = 0; i < numProps; ++i) {
      std::string kode, jenis, pemilikName, statusStr, nbuildStr;
      int fmult = 1, fdur = 0;
      if (!(in >> kode >> jenis >> pemilikName >> statusStr >> fmult >> fdur >> nbuildStr))
        throw std::runtime_error("Format properti tidak valid.");
      in.ignore();

      PetakProperti *prop = nullptr;
      for (auto &petak : board) {
        if (petak && petak->getShortName() == kode) {
          prop = dynamic_cast<PetakProperti *>(petak.get());
          break;
        }
      }
      if (!prop) continue;

      if (pemilikName == "BANK") {
        prop->setOwner(nullptr);
        prop->setMortgaged(false);
      } else {
        Player *owner = nullptr;
        for (auto &pl : players) {
          if (pl.getName() == pemilikName) { owner = &pl; break; }
        }
        if (owner) {
          prop->setOwner(owner);
          owner->addProperty(prop);
          prop->setMortgaged(statusStr == "MORTGAGED");
        }
      }

      if (auto *s = dynamic_cast<Street *>(prop)) {
        s->setFestivalState(fmult, fdur);
        if (nbuildStr == "H") {
          s->restoreBuildingState(4, true); 
        } else {
          int houses = std::stoi(nbuildStr);
          s->restoreBuildingState(houses, false);
        }
      }
    }

    int numLog = 0;
    if (!(in >> numLog)) { /* log mungkin kosong */ numLog = 0; }
    in.ignore();
    gm.clearLog();
    for (int i = 0; i < numLog; ++i) {
      std::string entry;
      if (!std::getline(in, entry)) break;
      gm.addLogEntry(entry);
    }

  } catch (const std::exception &e) {
    std::cout << "Gagal memuat file! " << e.what() << "\n";
    return false;
  }

  return true;
}
