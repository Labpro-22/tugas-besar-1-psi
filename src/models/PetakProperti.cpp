#include "models/PetakProperti.hpp"
#include "models/Player.hpp"

PetakProperti::PetakProperti(int id, const std::string& name, int hargaBeli)
    : Petak(id, name), owner(nullptr), hargaBeli(hargaBeli), isMortgaged(false) {}

Player* PetakProperti::getOwner() const { return owner; }
void PetakProperti::setOwner(Player* newOwner) { owner = newOwner; }

int PetakProperti::getHargaBeli() const { return hargaBeli; }

bool PetakProperti::getIsMortgaged() const { return isMortgaged; }
void PetakProperti::setMortgaged(bool status) { isMortgaged = status; }

void PetakProperti::injak(Player& p) {
    // Basic logic for stepping on a property can be placed here,
    // though GameManager will likely handle interactive purchasing
    // and dynamic rent deduction (since precise diceRoll might be needed).
    (void)p; // Mark as unused to avoid compiler warnings for now
}
