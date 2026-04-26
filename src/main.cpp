#include "core/GameManager.hpp"
#include "utils/SaveManager.hpp"

#include <iostream>
#include <string>

int main() {
  GameManager game;
  game.loadConfig("data/property.txt");

  std::cout << "\n============================================\n";
  std::cout << "         WELCOME TO NIMONSPOLI !!!          \n";
  std::cout << "============================================\n";
  std::cout << "1. New Game\n";
  std::cout << "2. Load Game (MUAT)\n";
  std::cout << "Pilih (1/2): ";

  std::string pilihan;
  std::getline(std::cin, pilihan);

  if (pilihan == "2") {
    std::cout << "Masukkan nama file save: ";
    std::string filename;
    std::getline(std::cin, filename);

    std::cout << "Memuat permainan...\n";
    if (!SaveManager::loadGame(game, filename)) {
      std::cout << "Gagal memuat. Memulai permainan baru.\n";
      pilihan = "1";
    } else {
      std::cout << "Permainan berhasil dimuat.\n";
    }
  }

  if (pilihan != "2") {

    int numPlayers = 0;
    std::cout << "Masukkan jumlah pemain (2-4): ";
    std::cin >> numPlayers;
    if (numPlayers < 2 || numPlayers > 4) {
      std::cout << "Tidak valid. Default 2 pemain.\n";
      numPlayers = 2;
    }
    game.initPlayers(numPlayers);
  }

  game.startGame();
  return 0;
}
