#include "core/GameManager.hpp"
#include <iostream>
#include <string>

int main() {
    GameManager game;
    game.loadConfig("data/property.txt");

    std::cout << "\n============================================\n";
    std::cout << "       SELAMAT DATANG DI NIMONSPOLI!        \n";
    std::cout << "============================================\n";
    std::cout << "1. New Game\n";
    std::cout << "2. Load Game\n";
    std::cout << "Pilihan (1/2): ";

    std::string pilihan;
    std::getline(std::cin >> std::ws, pilihan);

    if (pilihan == "2") {
        std::cout << "Masukkan nama file save: ";
        std::string filename;
        std::getline(std::cin, filename);
        std::cout << "Memuat permainan...\n";
        if (!game.loadGame(filename)) {
            std::cout << "Load gagal. Memulai New Game sebagai fallback.\n";
            pilihan = "1";
        }
    }

    if (pilihan != "2") {
        int numPlayers = 0;
        std::cout << "Masukkan jumlah pemain (2-4): ";
        if (!(std::cin >> numPlayers) || numPlayers < 2 || numPlayers > 4) {
            std::cin.clear();
            std::cout << "Input tidak valid. Menggunakan 2 pemain.\n";
            numPlayers = 2;
        }
        game.initPlayers(numPlayers);
    }

    game.startGame();
    return 0;
}
