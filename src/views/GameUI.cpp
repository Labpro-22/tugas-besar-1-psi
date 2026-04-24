#include "views/GameUI.hpp"
#include "utils/FormatHelper.hpp"
#include <iostream>
#include <string>

void GameUI::showMessage(const std::string &msg) { std::cout << msg << "\n"; }

void GameUI::showInlineMessage(const std::string &msg) { std::cout << msg; }

int GameUI::promptTaxChoice(int flatAmount, int percentage, int totalWealth) {
  (void)totalWealth;
  std::cout << "Pilih opsi pembayaran pajak:\n";
  std::cout << "1. Bayar flat " << formatUang(flatAmount) << "\n";
  std::cout << "2. Bayar 10% dari total kekayaan (" << formatUang(percentage)
            << ")\n";
  std::cout << "(Pilih sebelum menghitung kekayaan!)\n";

  int pilihan = 0;
  while (true) {
    std::cout << "Pilihan (1/2): ";
    std::string input;
    if (!std::getline(std::cin, input))
      break;
    try {
      pilihan = std::stoi(input);
      if (pilihan == 1 || pilihan == 2)
        break;
    } catch (...) {
    }
    std::cout << "Input tidak valid. Masukkan 1 atau 2.\n";
  }

  std::cout << "\n";
  return pilihan;
}

std::string GameUI::promptInput(const std::string &promptTxt) {
  std::cout << promptTxt;
  std::string input;
  std::getline(std::cin, input);
  return input;
}
