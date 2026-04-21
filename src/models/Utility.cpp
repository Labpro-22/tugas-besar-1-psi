#include "models/Utility.hpp"
#include "models/Player.hpp"

Utility::Utility(int id, const std::string& name, int hargaBeli)
    : PetakProperti(id, name, hargaBeli) {}

int Utility::getSewa(int diceRoll) const {
    if (isMortgaged || !owner) return 0;
    
    int count = 0;
    for (const auto& prop : owner->getOwnedProperties()) {
        if (dynamic_cast<Utility*>(prop)) {
            count++;
        }
    }
    
    // Example standard Monopoly rule: 1 Utility = 4x dice roll, 2 Utilities = 10x dice roll
    if (count == 1) {
        return diceRoll * 4;
    } 
    else if (count == 2) {
        return diceRoll * 10;
    }
    
    return 0;
}
