#include "models/Card.hpp"
#include "models/Player.hpp"
#include "utils/GameConstants.hpp"
#include "views/IGameUI.hpp"
#include "utils/FormatHelper.hpp"
#include "models/Street.hpp"
#include "models/Petak.hpp"
#include "models/Railroad.hpp"
#include "models/PetakProperti.hpp"
Card::Card(const std::string &desc) : description(desc) {}

std::string Card::getDescription() const { return description; }

MoneyCard::MoneyCard(const std::string &desc, int amount)
    : Card(desc), amount(amount) {}

#include "core/PaymentManager.hpp"

void MoneyCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
  if (amount > 0) {
    p.addMoney(amount);
    ui.showMessage("Kamu menerima M" + std::to_string(amount) + " dari Bank. Sisa Uang = M" + std::to_string(p.getMoney()) + ".");
  } else {
    if (board && players) {
        int amt = -amount;
        if (p.getMoney() >= amt) {
            p.reduceMoney(amt);
            ui.showMessage("Kamu membayar M" + std::to_string(amt) + " ke Bank. Sisa Uang = M" + std::to_string(p.getMoney()) + ".");
        } else {
            PaymentManager::processPayment(p, nullptr, amt, ui, *board, *players, "");
        }
    } else {
        p.reduceMoney(-amount);
    }
  }
}

MoveCard::MoveCard(const std::string &desc, int targetPosition)
    : Card(desc), targetPosition(targetPosition) {}



void MoveCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) { 
    p.setPosition(targetPosition); 
    if (board && targetPosition >= 0 && targetPosition < (int)board->size()) {
        ui.showMessage("Bidak dipindahkan ke " + (*board)[targetPosition]->getShortName() + ".");
        (*board)[targetPosition]->injak(p, ui, 0, board, players);
    }
}

JailCard::JailCard(const std::string &desc) : Card(desc) {}

void JailCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
  p.setPosition(GameConstants::JAIL_POSITION);
  p.setStatus(PlayerStatus::IN_JAIL);
}

RelativeMoveCard::RelativeMoveCard(const std::string &desc, int steps)
    : Card(desc), steps(steps) {}

void RelativeMoveCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
    if (board && !board->empty()) {
        int boardSize = (int)board->size();
        int newPos = (p.getPosition() + steps) % boardSize;
        if (newPos < 0) newPos += boardSize;
        p.setPosition(newPos);
        ui.showMessage("Bidak dipindahkan ke " + (*board)[newPos]->getShortName() + ".");
        (*board)[newPos]->injak(p, ui, 0, board, players);
    }
}

NearestStationCard::NearestStationCard(const std::string &desc) : Card(desc) {}

void NearestStationCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
    if (board && !board->empty()) {
        int boardSize = (int)board->size();
        int curPos = p.getPosition();
        int newPos = curPos;
        for (int i = 1; i <= boardSize; ++i) {
            int checkPos = (curPos + i) % boardSize;
            if (dynamic_cast<Railroad*>((*board)[checkPos].get())) {
                newPos = checkPos;
                break;
            }
        }
        if (newPos < curPos) {
            p.addMoney(GameConstants::GO_BONUS);
            ui.showMessage("[!] Melewati GO! Mendapat bonus " + formatUang(GameConstants::GO_BONUS) + ".");
        }
        p.setPosition(newPos);
        ui.showMessage("Bidak dipindahkan ke " + (*board)[newPos]->getShortName() + ".");
        (*board)[newPos]->injak(p, ui, 0, board, players);
    }
}

TransferPlayersCard::TransferPlayersCard(const std::string &desc, int amount)
    : Card(desc), amountPerPlayer(amount) {}

void TransferPlayersCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
    if (players) {
        if (amountPerPlayer > 0) {
            for (auto &other : *players) {
                if (&other != &p && other.getStatus() != PlayerStatus::BANKRUPT) {
                    PaymentManager::processPayment(other, &p, amountPerPlayer, ui, *board, *players, "");
                }
            }
        } else if (amountPerPlayer < 0) {
            int amt = -amountPerPlayer;
            for (auto &other : *players) {
                if (&other != &p && other.getStatus() != PlayerStatus::BANKRUPT) {
                    PaymentManager::processPayment(p, &other, amt, ui, *board, *players, "");
                }
            }
        }
    }
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

void SpecialCard::applyEffect(Player &p, IGameUI &ui, std::vector<std::unique_ptr<Petak>>* board, std::vector<Player>* players) {
    if (cardType == SpecialCardType::MOVE) {
        int boardSize = board ? board->size() : 40;
        int newPos = (p.getPosition() + value) % boardSize;
        if (newPos < 0) newPos += boardSize;
        p.setPosition(newPos);
        if (board && !board->empty()) {
            ui.showMessage("Bidak dipindahkan ke " + (*board)[newPos]->getShortName() + ".");
            (*board)[newPos]->injak(p, ui, 0, board, players);
        }
    } else if (cardType == SpecialCardType::DISCOUNT) {
        p.setDiscount(value);
    } else if (cardType == SpecialCardType::SHIELD) {
        p.setShieldActive(true);
    } else if (cardType == SpecialCardType::TELEPORT) {
        if (board) {
            ui.showMessage("Daftar Petak:");
            for (size_t i = 0; i < board->size(); ++i) {
                ui.showMessage(std::to_string(i) + ". " + (*board)[i]->getShortName() + " (" + (*board)[i]->getName() + ")");
            }
            std::string choiceStr = ui.promptInput("Pilih petak tujuan (0-" + std::to_string(board->size() - 1) + "): ");
            int choice = 0;
            try { choice = std::stoi(choiceStr); } catch (...) {}
            if (choice >= 0 && choice < (int)board->size()) {
                p.setPosition(choice);
                ui.showMessage("Bidak dipindahkan ke " + (*board)[choice]->getShortName() + ".");
                (*board)[choice]->injak(p, ui, 0, board, players);
            } else {
                ui.showMessage("Pilihan tidak valid, teleportasi dibatalkan.");
            }
        }
    } else if (cardType == SpecialCardType::LASSO) {
        if (players && board) {
            std::vector<Player*> opponents;
            ui.showMessage("Pemain lawan yang bisa ditarik:");
            for (auto &other : *players) {
                if (&other != &p && other.getStatus() != PlayerStatus::BANKRUPT) {
                    opponents.push_back(&other);
                    ui.showMessage(std::to_string(opponents.size()) + ". " + other.getName());
                }
            }
            if (opponents.empty()) {
                ui.showMessage("Tidak ada pemain lawan yang aktif.");
                return;
            }
            std::string choiceStr = ui.promptInput("Pilih pemain (1-" + std::to_string(opponents.size()) + "): ");
            int choice = 0;
            try { choice = std::stoi(choiceStr); } catch (...) {}
            if (choice >= 1 && choice <= (int)opponents.size()) {
                Player *target = opponents[choice - 1];
                target->setPosition(p.getPosition());
                ui.showMessage(target->getName() + " ditarik ke " + (*board)[p.getPosition()]->getShortName() + ".");
                (*board)[p.getPosition()]->injak(*target, ui, 0, board, players);
            } else {
                ui.showMessage("Pilihan tidak valid.");
            }
        }
    } else if (cardType == SpecialCardType::DEMOLITION) {
        if (players) {
            std::vector<PetakProperti*> targetProps;
            ui.showMessage("Properti lawan:");
            for (auto &other : *players) {
                if (&other != &p && other.getStatus() != PlayerStatus::BANKRUPT) {
                    for (auto *prop : other.getOwnedProperties()) {
                        targetProps.push_back(prop);
                        ui.showMessage(std::to_string(targetProps.size()) + ". " + prop->getShortName() + " (milik " + other.getName() + ")");
                    }
                }
            }
            if (targetProps.empty()) {
                ui.showMessage("Tidak ada properti lawan.");
                return;
            }
            std::string choiceStr = ui.promptInput("Pilih properti yang ingin dihancurkan (1-" + std::to_string(targetProps.size()) + "): ");
            int choice = 0;
            try { choice = std::stoi(choiceStr); } catch (...) {}
            if (choice >= 1 && choice <= (int)targetProps.size()) {
                PetakProperti *targetProp = targetProps[choice - 1];
                Player *targetOwner = targetProp->getOwner();
                
               
                if (auto *street = dynamic_cast<Street*>(targetProp)) {
                    street->demolishAllBuildings();
                }
                if (targetOwner) {
                    targetOwner->removeProperty(targetProp);
                }
                targetProp->lelang();
                ui.showMessage("Properti " + targetProp->getShortName() + " berhasil dihancurkan dan dikembalikan ke Bank!");
            } else {
                ui.showMessage("Pilihan tidak valid.");
            }
        }
    }
}
