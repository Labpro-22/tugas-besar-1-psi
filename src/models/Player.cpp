#include "models/Player.hpp"

Player::Player(int id, const std::string& name, int startingMoney)
    : id(id), name(name), money(startingMoney), position(0), status(PlayerStatus::ACTIVE) {}

int Player::getId() const { return id; }
std::string Player::getName() const { return name; }

int Player::getMoney() const { return money; }

void Player::addMoney(int amount) { 
    money += amount; 
}

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

void Player::addProperty(PetakProperti* property) {
    ownedProperties.push_back(property);
}

const std::vector<PetakProperti*>& Player::getOwnedProperties() const {
    return ownedProperties;
}
