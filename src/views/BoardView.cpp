#include "views/BoardView.hpp"
#include "models/Petak.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Street.hpp"
#include "utils/ColorRegistry.hpp"
#include <iostream>
#include <string>

std::pair<std::string, std::string>
getCellStrings(int pos, const std::vector<std::unique_ptr<Petak>> &board,
               const std::vector<Player> &players) {
  if (pos < 0 || pos >= static_cast<int>(board.size()))
    return {"            ", "            "};

  Petak *p = board[pos].get();
  std::string colorCode = p->getColorCode();
  std::string shortName = p->getShortName();

  std::string ansi = ColorRegistry::getAnsiColor(colorCode);
  std::string reset = "\033[0m";

  std::string line1 = "[" + colorCode + "] " + shortName;
  while (line1.length() < 12)
    line1 += " ";
  if (line1.length() > 12)
    line1 = line1.substr(0, 12);
  std::string visualLine1 = ansi + line1 + reset;

  std::string line2 = "";
  if (auto prop = dynamic_cast<PetakProperti *>(p)) {
    if (Player *owner = prop->getOwner()) {
      line2 += "P" + std::to_string(owner->getId());
      if (auto str = dynamic_cast<Street *>(prop)) {
        int hc = str->getHouseCount();
        if (str->isHotelBuilt()) {
          line2 += " *";
        } else if (hc > 0) {
          line2 += " ";
          for (int i = 0; i < hc; i++)
            line2 += "^";
        }
      }
    }
  }

  std::string tokens = "";
  for (const auto &player : players) {
    if (player.getPosition() == pos) {
      tokens += "(" + std::to_string(player.getId()) + ")";
    }
  }

  if (!tokens.empty()) {
    if (!line2.empty())
      line2 += " ";
    line2 += tokens;
  }

  while (line2.length() < 12)
    line2 += " ";
  if (line2.length() > 12)
    line2 = line2.substr(0, 12);

  return {visualLine1, ansi + line2 + reset};
}

void BoardView::printBoard(const std::vector<std::unique_ptr<Petak>> &board,
                           const std::vector<Player> &players, int turnCount) {
  if (board.size() < 40)
    return;

  auto printHorizBorderFull = []() {
    for (int i = 0; i < 11; i++)
      std::cout << "+------------";
    std::cout << "+\n";
  };

  std::cout << "\n> CETAK_PAPAN\n\n";

  printHorizBorderFull();
  for (int line = 1; line <= 2; line++) {
    for (int i = 20; i <= 30; i++) {
      std::cout << "|";
      auto strs = getCellStrings(i, board, players);
      std::cout << (line == 1 ? strs.first : strs.second);
    }
    std::cout << "|\n";
  }
  printHorizBorderFull();

  // Lebar total: 11 kotak * 13 karakter/kotak + 1 = 144 karakter.
  // Lebar tengah (tanpa 2 kotak di ujung): 144 - 1 (kiri) - 12 (sel) - 1
  // (kanan) - 12 (sel) - 2 (batas) = 116 spasi
  std::vector<std::string> centerStr(18, std::string(116, ' '));
  auto putText = [&](int row, const std::string &txt) {
    if (row < 0 || row >= static_cast<int>(centerStr.size()))
      return;
    int col = (116 - static_cast<int>(txt.length())) / 2; // Rata tengah
    for (size_t i = 0; i < txt.length(); i++) {
      int c = col + static_cast<int>(i);
      if (c >= 0 && c < 116) {
        centerStr[row][c] = txt[i];
      }
    }
  };

  putText(2, "========================================");
  putText(3, "||             NIMONSPOLI             ||");
  putText(4, "========================================");

  putText(6, "TURN " + std::to_string(turnCount) + " / 50");

  putText(8, "--------------------------------------------------");
  putText(9, "LEGENDA KEPEMILIKAN & STATUS");
  putText(10, "P1-P4 : Properti milik Pemain 1-4");
  putText(11, "^     : Rumah Level 1");
  putText(12, "^^    : Rumah Level 2");
  putText(13, "^^^   : Rumah Level 3");
  putText(14, "*     : Hotel (Maksimal)");
  putText(15, "(1)-(4) : Bidak (Posisi Pemain saat ini)");
  putText(16, "--------------------------------------------------");

  for (int r = 1; r <= 9; r++) {
    int leftPos = 20 - r;
    int rightPos = 30 + r;

    auto leftStrs = getCellStrings(leftPos, board, players);
    auto rightStrs = getCellStrings(rightPos, board, players);

    std::cout << "|" << leftStrs.first << "|" << centerStr[(r - 1) * 2] << "|"
              << rightStrs.first << "|\n";
    std::cout << "|" << leftStrs.second << "|" << centerStr[(r - 1) * 2 + 1]
              << "|" << rightStrs.second << "|\n";

    if (r < 9) {
      std::cout << "+------------+" << std::string(116, ' ')
                << "+------------+\n";
    }
  }

  printHorizBorderFull();
  for (int line = 1; line <= 2; line++) {
    for (int i = 10; i >= 0; i--) {
      std::cout << "|";
      auto strs = getCellStrings(i, board, players);
      std::cout << (line == 1 ? strs.first : strs.second);
    }
    std::cout << "|\n";
  }
  printHorizBorderFull();
}
