#ifndef TURNCONTROLLER_HPP
#define TURNCONTROLLER_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Petak;
class Player;
class IGameUI;
class CardDeck;
class Dice;

class TurnController {
private:
  std::vector<std::unique_ptr<Petak>> &board;
  std::vector<Player> &players;
  int &currentPlayerIndex;
  int &turnCount;
  CardDeck &chanceDeck;
  CardDeck &chestDeck;
  IGameUI &ui;
  bool &hasDoneAction;
  Dice &dice;

  std::function<bool(const std::string &)> saveCallback;

  void executePostRoll(Player &p, int roll1, int roll2, int &doublesCount,
                       bool &endTurnFlag);

public:
  TurnController(std::vector<std::unique_ptr<Petak>> &board,
                 std::vector<Player> &players, int &currentPlayerIndex,
                 int &turnCount, CardDeck &chanceDeck, CardDeck &chestDeck,
                 IGameUI &ui, bool &hasDoneAction, Dice &dice,
                 std::function<bool(const std::string &)> saveCallback);

  void playTurn();
};

#endif
