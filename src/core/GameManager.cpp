#include "core/GameManager.hpp"
#include "utils/ConfigParser.hpp"
#include "models/PetakProperti.hpp"
#include "models/PetakAksi.hpp"
#include "views/BoardView.hpp"

#include <iostream>
#include <limits>

GameManager::GameManager() 
    : chanceDeck("Chance"), chestDeck("Community Chest"), currentPlayerIndex(0), turnCount(1) {
    chanceDeck.addCard(Card("Advance to GO (Collect 200)", CardEffectType::MOVE_TO, 0));
    chanceDeck.addCard(Card("Bank pays you dividend of 50", CardEffectType::ADD_MONEY, 50));
    chestDeck.addCard(Card("Doctor's fee. Pay 50", CardEffectType::DEDUCT_MONEY, 50));
    
    chanceDeck.shuffleDeck();
    chestDeck.shuffleDeck();
}

void GameManager::loadConfig(const std::string& filename) {
    std::cout << "[GameManager] Loading board configuration...\n";
    board = ConfigParser::loadBoardConfig(filename);
    std::cout << "[GameManager] Board loaded with " << board.size() << " spaces.\n";
}

void GameManager::initPlayers(int numPlayers) {
    players.reserve(numPlayers); 

    // Handle leftover newline from previous std::cin numeric reads 
    // to prevent skipping the first player's name prompt
    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }

    for (int i = 0; i < numPlayers; ++i) {
        std::string name;
        std::cout << "Enter name for Player " << (i + 1) << ": ";
        std::getline(std::cin, name);
        players.emplace_back(i + 1, name, 1500); 
    }
}

void GameManager::startAuction(PetakProperti* prop) {
    std::cout << "[AUCTION] Starting auction for " << prop->getName() << "... (Dummy Text)\n";
    // Real auction logic will be implemented here later
}

void GameManager::playTurn() {
    Player& p = players[currentPlayerIndex];
    if (p.getStatus() == PlayerStatus::BANKRUPT) {
        return; // Skip bankrupt players
    }

    std::cout << "\n---------------------------------------------\n";
    std::cout << "Turn " << turnCount << " | Player: " << p.getName() 
              << " | Balancing: $" << p.getMoney() << "\n";
    std::cout << "---------------------------------------------\n";

    if (p.getStatus() == PlayerStatus::IN_JAIL) {
        std::cout << "[!] " << p.getName() << " is in JAIL! Skipping turn...\n";
        p.setStatus(PlayerStatus::ACTIVE); // Temporary simplified release logic
        return;
    }

    int doublesCount = 0;
    while (true) {
        std::cout << "> Masukkan perintah (CETAK_PAPAN / LEMPAR_DADU): ";
        std::string command;
        std::getline(std::cin, command); 

        if (command == "CETAK_PAPAN") {
            BoardView::printBoard(board, players, turnCount);
            continue; 
        }

        if (command != "LEMPAR_DADU" && command != "") { // allow empty enter as shortcut for test friendliness
            std::cout << "[!] Perintah tidak valid.\n";
            continue;
        }

        std::cout << "\nMengocok dadu...\n";
        int rollTotal = dice.roll();
        auto rollValues = dice.getLastRoll();
        
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "Hasil: " << rollValues.first << " + " << rollValues.second << " = " << rollTotal << "\n";

        if (dice.isDouble()) {
            doublesCount++;
            std::cout << "Angka kembar! (" << doublesCount << "/3)\n";
            if (doublesCount == 3) {
                std::cout << "[!] 3 Kali KEMBAR! Bidak terlempar ke Penjara!\n";
                p.setPosition(10);
                p.setStatus(PlayerStatus::IN_JAIL);
                break;
            }
        }

        if (board.empty()) {
            std::cerr << "CRITICAL ERROR: Board is not properly initialized.\n";
            return;
        }

        int previousPosition = p.getPosition();
        p.move(rollTotal, static_cast<int>(board.size()));
        
        std::cout << "Memajukan Bidak " << p.getName() << " sebanyak " << rollTotal << " petak...\n";
        
        if (p.getPosition() < previousPosition) {
            std::cout << "Melewati GO! Mendapat bonus $200.\n";
            p.addMoney(200);
        }

        Petak* currentSpace = board[p.getPosition()].get();
        std::cout << "Bidak mendarat di: " << currentSpace->getName() << "\n\n";

        // Let the space execute its native injak logic (base functionality)
        currentSpace->injak(p);

        // Core interactive logic
        if (auto prop = dynamic_cast<PetakProperti*>(currentSpace)) {
            if (prop->getOwner() == nullptr) {
                // Manual Purchasing Prompt
                std::cout << "Do you want to buy " << prop->getName() << " for $" << prop->getHargaBeli() << "? (Y/N): ";
                std::string choice;
                std::getline(std::cin, choice);

                if (choice == "Y" || choice == "y") {
                    if (p.getMoney() >= prop->getHargaBeli()) {
                        std::cout << "[ACTION] Bought " << prop->getName() << " for $" << prop->getHargaBeli() << ".\n";
                        p.reduceMoney(prop->getHargaBeli());
                        prop->setOwner(&p);
                        p.addProperty(prop);
                    } else {
                        std::cout << "[!] Insufficient funds.\n";
                        std::cout << "[!] Property declined. Triggering Auction...\n";
                        startAuction(prop);
                    }
                } else {
                    std::cout << "[!] Property declined. Triggering Auction...\n";
                    startAuction(prop);
                }

            } else if (prop->getOwner() != &p) {
                int rent = prop->getSewa(rollTotal);
                if (rent > 0 && !prop->getIsMortgaged()) {
                    std::cout << "[ACTION] Owned by " << prop->getOwner()->getName() << ". Paying rent: $" << rent << "\n";
                    p.reduceMoney(rent);
                    prop->getOwner()->addMoney(rent);
                }
            }
        } else if (auto aksi = dynamic_cast<PetakAksi*>(currentSpace)) {
            // Card Deck Integration
            std::string name = aksi->getName();
            if (name == "Chance" || name == "Kesempatan") {
                if (!chanceDeck.isEmpty()) {
                    Card drawn = chanceDeck.drawCard();
                    std::cout << "[CHANCE CARD] " << drawn.getDescription() << "\n";
                    drawn.applyEffect(p);
                }
            } else if (name == "Community Chest" || name == "Dana Umum") {
                if (!chestDeck.isEmpty()) {
                    Card drawn = chestDeck.drawCard();
                    std::cout << "[COMMUNITY CHEST] " << drawn.getDescription() << "\n";
                    drawn.applyEffect(p);
                }
            }
        }
        
        if (p.getStatus() == PlayerStatus::BANKRUPT) {
            std::cout << "!!! " << p.getName() << " WENT BANKRUPT !!!\n";
            break;
        }

        if (!dice.isDouble()) {
            break; // End turn
        } else {
             std::cout << "> Rolling again due to doubles...\n";
        }
    }
}

void GameManager::nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    if (currentPlayerIndex == 0) {
        turnCount++;
    }
}

Player* GameManager::checkWinner() {
    Player* winner = nullptr;
    int activeCount = 0;

    for (auto& p : players) {
        if (p.getStatus() != PlayerStatus::BANKRUPT) {
            activeCount++;
            winner = &p;
        }
    }

    if (activeCount <= 1 && players.size() > 1) {
        return winner;
    }
    return nullptr;
}

void GameManager::startGame() {
    std::cout << "\n============================================\n";
    std::cout << "         WELCOME TO NIMONSPOLI !!!          \n";
    std::cout << "============================================\n";

    while (true) {
        playTurn();
        
        Player* winner = checkWinner();
        if (winner != nullptr) {
            std::cout << "\n============================================\n";
            std::cout << "     " << winner->getName() << " IS THE WINNER!\n";
            std::cout << "============================================\n";
            break;
        }
        
        nextTurn();
    }
}
