#include "utils/SaveManager.hpp"
#include "core/GameManager.hpp"
#include "models/PetakProperti.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

bool SaveManager::saveGame(const GameManager &gm, const std::string &filename) {
  if (std::filesystem::exists(filename)) {
    std::cout << "File " << filename << " sudah ada. Timpa file lama? (y/n): ";
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

  out << "=== NIMONSPOLI ===\n";
  out << "Turn: " << gm.getTurnCount() << "\n";
  out << "Current Player Index: " << gm.getCurrentPlayerIndex() << "\n\n";

  out << "--- PLAYERS ---\n";
  for (const auto &p : gm.getPlayers()) {
    out << "Player " << p.getId() << ": " << p.getName() << "\n";
    out << "Money: " << p.getMoney() << "\n";
    out << "Position: " << p.getPosition() << "\n";
    out << "Status: ";
    if (p.getStatus() == PlayerStatus::ACTIVE)
      out << "ACTIVE\n";
    else if (p.getStatus() == PlayerStatus::IN_JAIL)
      out << "IN_JAIL\n";
    else
      out << "BANKRUPT\n";

    out << "Properties Owned: ";
    const auto &props = p.getOwnedProperties();
    if (props.empty()) {
      out << "None\n";
    } else {
      for (auto prop : props) {
        out << prop->getShortName() << (prop->getIsMortgaged() ? "[M] " : " ");
      }
      out << "\n";
    }
    out << "\n";
  }

  out.close();
  return true;
}

bool SaveManager::loadGame(GameManager & /*gm*/,
                           const std::string & /*filename*/) {
  return false;
}
