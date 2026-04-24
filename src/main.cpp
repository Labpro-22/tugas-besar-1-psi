#include "core/GameManager.hpp"
#include <iostream>

int main() {
  GameManager game;

  game.loadConfig("data/property.txt");

  int numPlayers;
  std::cout << "\nEnter number of players (e.g. 2): ";

  if (!(std::cin >> numPlayers) || numPlayers < 2) {
    std::cin.clear();
    std::cout << "Invalid. Falling back to 2 players.\n";
    numPlayers = 2;
  }

  game.initPlayers(numPlayers);
  game.startGame();

  return 0;
}