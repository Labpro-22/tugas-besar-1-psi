#include "models/Railroad.hpp"
#include "models/Player.hpp"

Railroad::Railroad(int id, const std::string& name, int hargaBeli, int baseRent)
    : PetakProperti(id, name, hargaBeli), baseRent(baseRent) {}

int Railroad::getSewa(int /*diceRoll*/) const {
    if (isMortgaged || !owner) return 0;
    
    int count = 0;
    for (const auto& prop : owner->getOwnedProperties()) {
        if (dynamic_cast<Railroad*>(prop)) {
            count++;
        }
    }
    
    if (count == 0) return 0;
    
    // Rent doubles for each additional railroad: 1 = base, 2 = base*2, 3 = base*4, 4 = base*8
    int multiplier = 1 << (count - 1); 
    return baseRent * multiplier;
}
