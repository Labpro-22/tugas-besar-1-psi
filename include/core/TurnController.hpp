#ifndef TURNCONTROLLER_HPP
#define TURNCONTROLLER_HPP

#include "models/CardDeck.hpp"
#include "models/Dice.hpp"
#include "models/Petak.hpp"
#include "models/Player.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class IGameUI;

class SpecialCard;

class TurnController {
private:
  std::vector<std::unique_ptr<Petak>> &board;
  std::vector<Player>                 &players;
  int      &currentPlayerIndex;
  int      &turnCount;
  CardDeck &chanceDeck;
  CardDeck &chestDeck;
  IGameUI  &ui;
  bool     &hasDoneAction;
  Dice     &dice;

  const std::vector<std::string> &transactionLog;

  std::function<bool(const std::string &)>  cbSave;
  std::function<void(const std::string &)>  cbLog;
  std::function<void *()>     cbDraw;   
  std::function<void(void *)> cbDiscard; 

  void addLog(const std::string &jenis, const std::string &detail,
              const std::string &username);
  void executePostRoll(Player &p, int roll1, int roll2,
                       int &doublesCount, bool &endTurnFlag);
  void handleTurnStart(Player &p);
  void handleDropCard(Player &p);
  void printLog(int n) const;

public:
  TurnController(
      std::vector<std::unique_ptr<Petak>> &board,
      std::vector<Player>                 &players,
      int      &currentPlayerIndex,
      int      &turnCount,
      CardDeck &chanceDeck,
      CardDeck &chestDeck,
      IGameUI  &ui,
      bool     &hasDoneAction,
      Dice     &dice,
      std::function<bool(const std::string &)>  cbSave,
      std::function<void(const std::string &)>  cbLog,
      std::function<void *()>     cbDraw,
      std::function<void(void *)> cbDiscard,
      const std::vector<std::string>           &transactionLog);

  void playTurn();
};

#endif
