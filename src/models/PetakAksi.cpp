#include "models/PetakAksi.hpp"
#include "models/Player.hpp"

PetakAksi::PetakAksi(int id, const std::string& name, ActionType type, int amount)
    : Petak(id, name), type(type), amount(amount) {}

ActionType PetakAksi::getType() const { return type; }

int PetakAksi::getAmount() const { return amount; }

void PetakAksi::injak(Player& p) {
    switch (type) {
        case ActionType::GO:
            p.addMoney(amount > 0 ? amount : 200);
            break;
        case ActionType::JAIL:
            p.setStatus(PlayerStatus::IN_JAIL);
            p.setPosition(10); // Typically JAIL is at index 10 in standard board
            break;
        case ActionType::TAX:
            p.reduceMoney(amount);
            break;
        case ActionType::FREE_PARKING:
            break;
    }
}
