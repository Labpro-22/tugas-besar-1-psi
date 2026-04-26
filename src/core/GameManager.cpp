#include "core/GameManager.hpp"
#include "core/TurnController.hpp"
#include "models/Card.hpp"
#include "utils/ConfigParser.hpp"
#include "utils/FormatHelper.hpp"
#include "utils/GameConstants.hpp"
#include "utils/SaveManager.hpp"
#include "views/BoardView.hpp"
#include "views/GameUI.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>

GameManager::GameManager()
    : chanceDeck("Chance"), chestDeck("Community Chest"),
      currentPlayerIndex(0), turnCount(1), maxTurn(GameConstants::MAX_TURNS),
      hasDoneAction(false) {

  ui = std::make_unique<GameUI>();

  chanceDeck.addCard(
      std::make_unique<MoveCard>("Advance to GO (Collect 200)", 0));
  chanceDeck.addCard(
      std::make_unique<MoneyCard>("Bank pays you dividend of 50", 50));
  chestDeck.addCard(std::make_unique<MoneyCard>("Doctor's fee. Pay 50", -50));

  chanceDeck.shuffleDeck();
  chestDeck.shuffleDeck();

  initSpecialDeck();
}

void GameManager::initSpecialDeck() {
  specialDeck.clear();
  specialDiscard.clear();

  std::mt19937 rng(std::random_device{}());

  // 4 MoveCard (langkah 1-6 acak)
  for (int i = 0; i < 4; i++) {
    int steps = std::uniform_int_distribution<int>(1, 6)(rng);
    specialDeck.push_back(std::make_unique<SpecialCard>(SpecialCardType::MOVE, steps));
  }
  // 3 DiscountCard (diskon 10-50% acak)
  for (int i = 0; i < 3; i++) {
    int pct = std::uniform_int_distribution<int>(1, 5)(rng) * 10;
    specialDeck.push_back(std::make_unique<SpecialCard>(SpecialCardType::DISCOUNT, pct, 1));
  }
  // 2 ShieldCard
  for (int i = 0; i < 2; i++)
    specialDeck.push_back(std::make_unique<SpecialCard>(SpecialCardType::SHIELD));
  // 2 TeleportCard
  for (int i = 0; i < 2; i++)
    specialDeck.push_back(std::make_unique<SpecialCard>(SpecialCardType::TELEPORT));
  // 2 LassoCard
  for (int i = 0; i < 2; i++)
    specialDeck.push_back(std::make_unique<SpecialCard>(SpecialCardType::LASSO));
  // 2 DemolitionCard
  for (int i = 0; i < 2; i++)
    specialDeck.push_back(std::make_unique<SpecialCard>(SpecialCardType::DEMOLITION));

  std::shuffle(specialDeck.begin(), specialDeck.end(), rng);
}

void GameManager::reloadSpecialDeckFromDiscard() {
  for (auto &c : specialDiscard)
    specialDeck.push_back(std::move(c));
  specialDiscard.clear();

  std::mt19937 rng(std::random_device{}());
  std::shuffle(specialDeck.begin(), specialDeck.end(), rng);
}

std::unique_ptr<SpecialCard> GameManager::drawSpecialCard() {
  if (specialDeck.empty())
    reloadSpecialDeckFromDiscard();
  if (specialDeck.empty())
    return nullptr;

  auto card = std::move(specialDeck.front());
  specialDeck.erase(specialDeck.begin());
  return card;
}

void GameManager::discardSpecialCard(std::unique_ptr<SpecialCard> card) {
  if (card)
    specialDiscard.push_back(std::move(card));
}

void GameManager::addLog(const std::string &entry) {
  transactionLog.push_back(entry);
}

const std::vector<std::string> &GameManager::getLog() const {
  return transactionLog;
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
  return (active <= 1 && static_cast<int>(players.size()) > 1) ? winner
                                                                 : nullptr;
}

void GameManager::startGame() {
  ui->showMessage("\n============================================");
  ui->showMessage("         WELCOME TO NIMONSPOLI !!!          ");
  ui->showMessage("============================================");

  // cbDraw/cbDiscard menggunakan void* untuk menghindari instantiasi template
  // complex di header — di .cpp-nya di-cast ke SpecialCard*
  auto cbSave = [this](const std::string &fn) {
    return SaveManager::saveGame(*this, fn);
  };
  auto cbLog = [this](const std::string &e) { this->addLog(e); };
  auto cbDraw = [this]() -> void * {
    return static_cast<void *>(this->drawSpecialCard().release());
  };
  auto cbDiscard = [this](void *raw) {
    this->discardSpecialCard(
        std::unique_ptr<SpecialCard>(static_cast<SpecialCard *>(raw)));
  };

  TurnController turnCtrl(board, players, currentPlayerIndex, turnCount,
                          chanceDeck, chestDeck, *ui, hasDoneAction, dice,
                          cbSave, cbLog, cbDraw, cbDiscard,
                          transactionLog);

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
