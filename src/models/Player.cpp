#include "models/Player.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"
#include <algorithm>

Player::Player(int id, const std::string &name, int startingMoney)
    : id(id), name(name), money(startingMoney), position(0),
      status(PlayerStatus::ACTIVE) {}

int Player::getId() const { return id; }
std::string Player::getName() const { return name; }

int Player::getMoney() const { return money; }

void Player::addMoney(int amount) { money += amount; }

void Player::reduceMoney(int amount) {
  money -= amount;
  if (money < 0) {
    status = PlayerStatus::BANKRUPT;
  }
}

int Player::getPosition() const { return position; }

void Player::setPosition(int pos) { position = pos; }

void Player::move(int steps, int boardSize) {
  position = (position + steps) % boardSize;
}

PlayerStatus Player::getStatus() const { return status; }

void Player::setStatus(PlayerStatus newStatus) { status = newStatus; }

void Player::addProperty(PetakProperti *property) {
  ownedProperties.push_back(property);
}

void Player::removeProperty(PetakProperti *property) {
  ownedProperties.erase(
      std::remove(ownedProperties.begin(), ownedProperties.end(), property),
      ownedProperties.end());
}

void Player::clearProperties() {
  ownedProperties.clear();
}

const std::vector<PetakProperti *> &Player::getOwnedProperties() const {
  return ownedProperties;
}

int Player::getRailroadCount() const {
  int count = 0;
  for (auto prop : ownedProperties) {
    if (dynamic_cast<Railroad *>(prop))
      count++;
  }
  return count;
}

int Player::getUtilityCount() const {
  int count = 0;
  for (auto prop : ownedProperties) {
    if (dynamic_cast<Utility *>(prop))
      count++;
  }
  return count;
}

void Player::addSpecialCard(std::unique_ptr<SpecialCard> card) {
  hand.push_back(std::move(card));
}

void Player::removeSpecialCard(int index) {
  if (index >= 0 && index < static_cast<int>(hand.size()))
    hand.erase(hand.begin() + index);
}

const std::vector<std::unique_ptr<SpecialCard>> &Player::getHand() const {
  return hand;
}

int  Player::getHandSize() const { return static_cast<int>(hand.size()); }
bool Player::isHandFull()  const { return static_cast<int>(hand.size()) >= 3; }
