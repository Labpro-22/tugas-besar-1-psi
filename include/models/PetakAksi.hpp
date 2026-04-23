#ifndef PETAKAKSI_HPP
#define PETAKAKSI_HPP

#include "models/Petak.hpp"

enum class ActionType {
    GO,
    JAIL,
    TAX,
    FREE_PARKING,
    FESTIVAL
};

class PetakAksi : public Petak {
private:
    ActionType type;
    int amount; // Used for GO bonus or TAX penalty

public:
    PetakAksi(int id, const std::string& name, ActionType type, int amount = 0);
    
    ActionType getType() const;
    int getAmount() const;

    void injak(Player& p, int diceRoll = 0) override;
};

#endif
