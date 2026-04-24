#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
#include <string>
#include <vector>

class PetakProperti;

enum class PlayerStatus { ACTIVE, IN_JAIL, BANKRUPT };

class Player {
private:
  int id;
  std::string name;
  int money;
  int position;
  PlayerStatus status;

  std::vector<PetakProperti *> ownedProperties;

public:
  Player(int id, const std::string &name, int startingMoney = 1500);
  ~Player() = default;

  int getId() const;
  std::string getName() const;

  int getMoney() const;
  void addMoney(int amount);
  void reduceMoney(int amount);

  int getPosition() const;
  void setPosition(int pos);
  void move(int steps, int boardSize = 40);

  PlayerStatus getStatus() const;
  void setStatus(PlayerStatus newStatus);

  void addProperty(PetakProperti *property);
  const std::vector<PetakProperti *> &getOwnedProperties() const;

  int getRailroadCount() const;
  int getUtilityCount() const;
};

#endif
