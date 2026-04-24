#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <memory>
#include <string>
#include <vector>

#include "models/CardDeck.hpp"
#include "models/Dice.hpp"
#include "models/Petak.hpp"
#include "models/Player.hpp"
#include "views/IGameUI.hpp"

class GameManager {
    friend class SaveManager;

private:
  std::vector<Player> players;
  std::vector<std::unique_ptr<Petak>> board;
  Dice dice;
  CardDeck chanceDeck;
  CardDeck chestDeck;

  int currentPlayerIndex;
  int turnCount;
  bool hasDoneAction;

  std::unique_ptr<IGameUI> ui;

public:
  GameManager();

  void loadConfig(const std::string &filename);
  void initPlayers(int numPlayers);
  void startGame();
  void nextTurn();
  Player *checkWinner();

  const std::vector<Player> &getPlayers() const { return players; }
  const std::vector<std::unique_ptr<Petak>> &getBoard() const { return board; }
  int getCurrentPlayerIndex() const { return currentPlayerIndex; }
  int getTurnCount() const { return turnCount; }
};

#endif
