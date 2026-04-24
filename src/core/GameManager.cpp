#include "core/GameManager.hpp"
#include "core/TurnController.hpp"
#include "models/Card.hpp"
#include "utils/ConfigParser.hpp"
#include "utils/FormatHelper.hpp"
#include "utils/GameConstants.hpp"
#include "utils/SaveManager.hpp"
#include "views/BoardView.hpp"
#include "views/GameUI.hpp"

#include <functional>
#include <iostream>
#include <sstream>

GameManager::GameManager()
    : chanceDeck("Chance"), chestDeck("Community Chest"), currentPlayerIndex(0),
      turnCount(1), hasDoneAction(false) {

  ui = std::make_unique<GameUI>();

  chanceDeck.addCard(
      std::make_unique<MoveCard>("Advance to GO (Collect 200)", 0));
  chanceDeck.addCard(
      std::make_unique<MoneyCard>("Bank pays you dividend of 50", 50));
  chestDeck.addCard(std::make_unique<MoneyCard>("Doctor's fee. Pay 50", -50));

  chanceDeck.shuffleDeck();
  chestDeck.shuffleDeck();
}

void GameManager::loadConfig(const std::string &filename) {
  board = ConfigParser::loadBoardConfig(filename);
}

void GameManager::initPlayers(int numPlayers) {
  players.reserve(numPlayers);
  if (std::cin.peek() == '\n')
    std::cin.ignore();

  for (int i = 0; i < numPlayers; ++i) {
    std::string name = ui->promptInput("Enter name for Player " +
                                       std::to_string(i + 1) + ": ");
    players.emplace_back(i + 1, name, GameConstants::STARTING_MONEY);
  }
}

void GameManager::nextTurn() {
  currentPlayerIndex =
      (currentPlayerIndex + 1) % static_cast<int>(players.size());
  if (currentPlayerIndex == 0)
    turnCount++;
}

Player *GameManager::checkWinner() {
  Player *winner = nullptr;
  int active = 0;
  for (auto &p : players) {
    if (p.getStatus() != PlayerStatus::BANKRUPT) {
      active++;
      winner = &p;
    }
  }
  return (active <= 1 && players.size() > 1) ? winner : nullptr;
}

void GameManager::startGame() {
  ui->showMessage("\n============================================");
  ui->showMessage("         WELCOME TO NIMONSPOLI !!!          ");
  ui->showMessage("============================================");

  TurnController turnCtrl(board, players, currentPlayerIndex, turnCount,
                          chanceDeck, chestDeck, *ui, hasDoneAction, dice,
                          [this](const std::string &filename) {
                            return SaveManager::saveGame(*this, filename);
                          });
  while (true) {
    turnCtrl.playTurn();

    Player *winner = checkWinner();
    if (winner) {
      ui->showMessage("\n============================================");
      ui->showMessage("     " + winner->getName() + " IS THE WINNER!");
      ui->showMessage("============================================");
      break;
    }
    nextTurn();
  }
}

bool GameManager::loadGame(const std::string& filename) {
    return SaveManager::loadGame(*this, filename);
}
