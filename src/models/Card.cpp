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

SpecialCard::SpecialCard(SpecialCardType type, int value, int duration)
    : cardType(type), value(value), duration(duration) {}

SpecialCardType SpecialCard::getType()     const { return cardType; }
int             SpecialCard::getValue()    const { return value; }
int             SpecialCard::getDuration() const { return duration; }
void            SpecialCard::setDuration(int d)  { duration = d; }
void            SpecialCard::decrementDuration() { if (duration > 0) --duration; }

std::string SpecialCard::getTypeName() const {
  switch (cardType) {
  case SpecialCardType::MOVE:       return "MoveCard";
  case SpecialCardType::DISCOUNT:   return "DiscountCard";
  case SpecialCardType::SHIELD:     return "ShieldCard";
  case SpecialCardType::TELEPORT:   return "TeleportCard";
  case SpecialCardType::LASSO:      return "LassoCard";
  case SpecialCardType::DEMOLITION: return "DemolitionCard";
  }
  return "UnknownCard";
}

std::string SpecialCard::getDescription() const {
  switch (cardType) {
  case SpecialCardType::MOVE:
    return "Maju " + std::to_string(value) + " Petak";
  case SpecialCardType::DISCOUNT:
    return "Diskon " + std::to_string(value) + "%";
  case SpecialCardType::SHIELD:
    return "Kebal tagihan atau sanksi selama 1 giliran";
  case SpecialCardType::TELEPORT:
    return "Pindah ke petak manapun";
  case SpecialCardType::LASSO:
    return "Menarik lawan ke posisi kamu";
  case SpecialCardType::DEMOLITION:
    return "Menghancurkan properti milik lawan";
  }
  return "";
}
