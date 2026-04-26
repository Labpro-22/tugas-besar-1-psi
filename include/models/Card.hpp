#ifndef CARD_HPP
#define CARD_HPP

#include <string>
#include <vector>
#include <memory>

class Player;
class IGameUI;
class Petak;

class Card {
protected:
  std::string description;

public:
  Card(const std::string &desc);
  virtual ~Card() = default;

  std::string getDescription() const;

  virtual void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) = 0;
};

class MoneyCard : public Card {
private:
  int amount;

public:
  MoneyCard(const std::string &desc, int amount);
  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

class MoveCard : public Card {
private:
  int targetPosition;

public:
  MoveCard(const std::string &desc, int targetPosition);
  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

class JailCard : public Card {
public:
  JailCard(const std::string &desc);
  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

class RelativeMoveCard : public Card {
private:
  int steps;
public:
  RelativeMoveCard(const std::string &desc, int steps);
  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

class NearestStationCard : public Card {
public:
  NearestStationCard(const std::string &desc);
  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

class TransferPlayersCard : public Card {
private:
  int amountPerPlayer;
public:
  TransferPlayersCard(const std::string &desc, int amount);
  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

enum class SpecialCardType {
  MOVE, DISCOUNT, SHIELD, TELEPORT, LASSO, DEMOLITION
};

class SpecialCard {
private:
  SpecialCardType cardType;
  int value;
  int duration;

public:
  SpecialCard(SpecialCardType type, int value = 0, int duration = 0);

  SpecialCardType getType()     const;
  int             getValue()    const;
  int             getDuration() const;
  void            setDuration(int d);
  void            decrementDuration();

  std::string getTypeName()    const;
  std::string getDescription() const;

  void applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr);
};

#endif
