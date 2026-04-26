#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "models/Card.hpp"
#include "models/CardDeck.hpp"
#include "models/Dice.hpp"
#include "models/Petak.hpp"
#include "models/Player.hpp"
#include "views/IGameUI.hpp"

#include <memory>
#include <string>
#include <vector>

class GameManager {
private:
  std::vector<Player> players;
  std::vector<std::unique_ptr<Petak>> board;
  Dice dice;
  CardDeck chanceDeck;
  CardDeck chestDeck;

  int currentPlayerIndex;
  int turnCount;
  int maxTurn;
  bool hasDoneAction;

  std::unique_ptr<IGameUI> ui;

  std::vector<std::unique_ptr<SpecialCard>> specialDeck;
  std::vector<std::unique_ptr<SpecialCard>> specialDiscard;

  std::vector<std::string> transactionLog;

  void initSpecialDeck();
  void reloadSpecialDeckFromDiscard();

public:
  GameManager();

  void loadConfig(const std::string &filename);
  void initPlayers(int numPlayers);
  void startGame();
  void nextTurn();
  Player *checkWinner();

  std::unique_ptr<SpecialCard> drawSpecialCard();
  void discardSpecialCard(std::unique_ptr<SpecialCard> card);

  void addLog(const std::string &entry);
  const std::vector<std::string> &getLog() const;


  const std::vector<Player>                  &getPlayers()  const { return players; }
  const std::vector<std::unique_ptr<Petak>>  &getBoard()    const { return board; }
  int getCurrentPlayerIndex() const { return currentPlayerIndex; }
  int getTurnCount()          const { return turnCount; }
  int getMaxTurn()            const { return maxTurn; }

  // Mutable getters (digunakan SaveManager::loadGame)
  std::vector<Player>                 &getPlayersRef()  { return players; }
  std::vector<std::unique_ptr<Petak>> &getBoardRef()    { return board; }
  void setCurrentPlayerIndex(int i)   { currentPlayerIndex = i; }
  void setTurnCount(int t)            { turnCount = t; }
  void setMaxTurn(int m)              { maxTurn   = m; }
  void clearLog()                     { transactionLog.clear(); }
  void addLogEntry(const std::string &e) { transactionLog.push_back(e); }
  std::vector<std::unique_ptr<SpecialCard>> &getSpecialDeckRef() { return specialDeck; }
};

#endif
