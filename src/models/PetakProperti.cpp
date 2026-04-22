#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Street.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"
#include <iostream>
#include <iomanip>
#include <string>

static std::string formatUangLocal(int amount) {
    std::string s = std::to_string(amount);
    int n = s.length();
    for (int i = n - 3; i > 0; i -= 3) {
        s.insert(i, ".");
    }
    return "M" + s;
}

PetakProperti::PetakProperti(int id, const std::string& name, int hargaBeli)
    : Petak(id, name), owner(nullptr), hargaBeli(hargaBeli), isMortgaged(false) {}

Player* PetakProperti::getOwner() const { return owner; }
void PetakProperti::setOwner(Player* newOwner) { owner = newOwner; }

int PetakProperti::getHargaBeli() const { return hargaBeli; }

bool PetakProperti::getIsMortgaged() const { return isMortgaged; }
void PetakProperti::setMortgaged(bool status) { isMortgaged = status; }

void PetakProperti::injak(Player& p, int diceRoll) {
    if (owner != nullptr && owner != &p) {
        std::cout << "Kamu mendarat di " << getName() << " (" << getShortName() << "), milik " << owner->getName() << "!\n\n";

        if (isMortgaged) {
            std::cout << "Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n";
        } else {
            int rent = getSewa(diceRoll);
            
            std::string kondisi = "Tanah Kosong";
            if (auto street = dynamic_cast<Street*>(this)) {
                if (street->getIsHotel()) kondisi = "Hotel";
                else if (street->getHouseCount() > 0) kondisi = std::to_string(street->getHouseCount()) + " rumah";
            } else if (dynamic_cast<Railroad*>(this)) {
                int count = 0;
                for (auto r : owner->getOwnedProperties()) {
                    if (dynamic_cast<Railroad*>(r)) count++;
                }
                kondisi = std::to_string(count) + " Stasiun";
            } else if (dynamic_cast<Utility*>(this)) {
                int count = 0;
                for (auto u : owner->getOwnedProperties()) {
                    if (dynamic_cast<Utility*>(u)) count++;
                }
                kondisi = std::to_string(count) + " Utilitas (Dadu x " + (count == 1 ? "4" : "10") + ")";
            }

            std::cout << "Kondisi     : " << kondisi << "\n";
            std::cout << "Sewa        : " << formatUangLocal(rent) << "\n\n";

            if (p.getMoney() >= rent) {
                int pMoneyBefore = p.getMoney();
                int oMoneyBefore = owner->getMoney();
                p.reduceMoney(rent);
                owner->addMoney(rent);
                
                std::cout << "Uang kamu       : " << std::left << std::setw(6) << formatUangLocal(pMoneyBefore) << " -> " << formatUangLocal(p.getMoney()) << "\n";
                std::cout << "Uang " << std::left << std::setw(10) << owner->getName() << " : " << std::left << std::setw(6) << formatUangLocal(oMoneyBefore) << " -> " << formatUangLocal(owner->getMoney()) << "\n";
            } else {
                std::cout << "---\n\n";
                std::cout << "Kamu tidak mampu membayar sewa penuh! (" << formatUangLocal(rent) << ")\n";
                std::cout << "Uang kamu saat ini: " << formatUangLocal(p.getMoney()) << "\n";
                std::cout << "// Alur dilanjutkan ke Kebangkrutan\n";
                
                int pMoneyBefore = p.getMoney();
                owner->addMoney(pMoneyBefore); 
                p.reduceMoney(rent);
            }
        }
    }
}
