#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Player;

enum class CardEffectType {
    ADD_MONEY,
    DEDUCT_MONEY,
    MOVE_TO,
    GO_TO_JAIL
};

class Card {
protected:
    std::string description;
    CardEffectType effectType;
    int value; // Money amount or board index

public:
    Card(const std::string& desc, CardEffectType type, int value);
    virtual ~Card() = default;

    std::string getDescription() const;
    CardEffectType getEffectType() const;
    int getValue() const;

    virtual void applyEffect(Player& p);
};

#endif
