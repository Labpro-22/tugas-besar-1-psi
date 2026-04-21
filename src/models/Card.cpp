#include "models/Card.hpp"
#include "models/Player.hpp"

Card::Card(const std::string& desc, CardEffectType type, int value)
    : description(desc), effectType(type), value(value) {}

std::string Card::getDescription() const { return description; }
CardEffectType Card::getEffectType() const { return effectType; }
int Card::getValue() const { return value; }

void Card::applyEffect(Player& p) {
    switch (effectType) {
        case CardEffectType::ADD_MONEY:
            p.addMoney(value);
            break;
        case CardEffectType::DEDUCT_MONEY:
            p.reduceMoney(value);
            break;
        case CardEffectType::MOVE_TO:
            p.setPosition(value);
            break;
        case CardEffectType::GO_TO_JAIL:
            p.setPosition(10); // Standard Jail position index
            p.setStatus(PlayerStatus::IN_JAIL);
            break;
    }
}
