#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include <iostream>

PetakProperti::PetakProperti(int id, const std::string& name, int hargaBeli)
    : Petak(id, name), owner(nullptr), hargaBeli(hargaBeli), isMortgaged(false) {}

Player* PetakProperti::getOwner() const { return owner; }
void PetakProperti::setOwner(Player* newOwner) { owner = newOwner; }

int PetakProperti::getHargaBeli() const { return hargaBeli; }

bool PetakProperti::getIsMortgaged() const { return isMortgaged; }
void PetakProperti::setMortgaged(bool status) { isMortgaged = status; }

void PetakProperti::injak(Player& p) {
    if (owner != nullptr && owner != &p && !isMortgaged) {
        // Catatan: getSewa(0) aman untuk Street dan Railroad, namun untuk Utility
        // mungkin akan me-return 0 jika kita tidak mem-passing dadu.
        int rent = getSewa(0); 
        if (rent > 0) {
            std::cout << "[ACTION] Owned by " << owner->getName() << ". Paying rent: $" << rent << "\n";
            p.reduceMoney(rent);
            owner->addMoney(rent);
        }
    }
}
