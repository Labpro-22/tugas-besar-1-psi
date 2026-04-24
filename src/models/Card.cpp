#include "models/Card.hpp"
#include "models/Player.hpp"
#include "utils/GameConstants.hpp"

Card::Card(const std::string &desc) : description(desc) {}

std::string Card::getDescription() const { return description; }

MoneyCard::MoneyCard(const std::string &desc, int amount)
    : Card(desc), amount(amount) {}

void MoneyCard::applyEffect(Player &p) {
  if (amount > 0) {
    p.addMoney(amount);
  } else {
    p.reduceMoney(-amount);
  }
}

MoveCard::MoveCard(const std::string &desc, int targetPosition)
    : Card(desc), targetPosition(targetPosition) {}

void MoveCard::applyEffect(Player &p) { p.setPosition(targetPosition); }

JailCard::JailCard(const std::string &desc) : Card(desc) {}

void JailCard::applyEffect(Player &p) {
  p.setPosition(GameConstants::JAIL_POSITION);
  p.setStatus(PlayerStatus::IN_JAIL);
}
