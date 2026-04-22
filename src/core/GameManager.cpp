#include "core/GameManager.hpp"
#include "utils/ConfigParser.hpp"
#include "models/PetakProperti.hpp"
#include "models/PetakAksi.hpp"
#include "views/BoardView.hpp"

#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include "models/Street.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"

GameManager::GameManager() 
    : chanceDeck("Chance"), chestDeck("Community Chest"), currentPlayerIndex(0), turnCount(1) {
    chanceDeck.addCard(Card("Advance to GO (Collect 200)", CardEffectType::MOVE_TO, 0));
    chanceDeck.addCard(Card("Bank pays you dividend of 50", CardEffectType::ADD_MONEY, 50));
    chestDeck.addCard(Card("Doctor's fee. Pay 50", CardEffectType::DEDUCT_MONEY, 50));
    
    chanceDeck.shuffleDeck();
    chestDeck.shuffleDeck();
}

static std::string formatUang(int amount) {
    std::string s = std::to_string(amount);
    int n = s.length();
    for (int i = n - 3; i > 0; i -= 3) {
        s.insert(i, ".");
    }
    return "M" + s;
}

void GameManager::loadConfig(const std::string& filename) {
    board = ConfigParser::loadBoardConfig(filename);
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

void GameManager::cetakAktaCommand() {
    std::string kode;
    std::cout << "Masukkan kode petak: ";
    std::getline(std::cin, kode);

    PetakProperti* targetProp = nullptr;
    for (const auto& p : board) {
        if (p && p->getShortName() == kode) {
            targetProp = dynamic_cast<PetakProperti*>(p.get());
            break;
        }
    }

    if (!targetProp) {
        std::cout << "Petak \"" << kode << "\" tidak ditemukan atau bukan properti.\n";
        return;
    }

    std::string colorName = targetProp->getColorCode();
    std::string fullColor = "DEFAULT";
    if (colorName == "CK") fullColor = "COKLAT";
    else if (colorName == "BM") fullColor = "BIRU MUDA";
    else if (colorName == "PK") fullColor = "MERAH MUDA";
    else if (colorName == "OR") fullColor = "ORANGE";
    else if (colorName == "MR") fullColor = "MERAH";
    else if (colorName == "KN") fullColor = "KUNING";
    else if (colorName == "HJ") fullColor = "HIJAU";
    else if (colorName == "BT") fullColor = "BIRU TUA";
    else if (colorName == "AB") fullColor = "ABU-ABU";

    std::string titleRow = "[" + fullColor + "] " + targetProp->getName() + " (" + kode + ")";
    if (titleRow.length() > 30) {
        titleRow = titleRow.substr(0, 27) + "..."; // Truncate safely
    }
    
    std::cout << "\n+================================+\n";
    std::cout << "|        AKTA KEPEMILIKAN        |\n";
    std::cout << "|  " << std::left << std::setw(30) << titleRow << "|\n";
    std::cout << "+================================+\n";
    
    int hargaBeli = targetProp->getHargaBeli();
    int nilaiGadai = hargaBeli / 2;

    std::cout << "| Harga Beli        : " << std::left << std::setw(11) << formatUang(hargaBeli) << "|\n";
    std::cout << "| Nilai Gadai       : " << std::left << std::setw(11) << formatUang(nilaiGadai) << "|\n";
    std::cout << "+--------------------------------+\n";

    if (auto street = dynamic_cast<Street*>(targetProp)) {
        const auto& rents = street->getRentPrices();
        std::cout << "| Sewa (unimproved) : " << std::left << std::setw(11) << (rents.size() > 0 ? formatUang(rents[0]) : "M0") << "|\n";
        std::cout << "| Sewa (1 rumah)    : " << std::left << std::setw(11) << (rents.size() > 1 ? formatUang(rents[1]) : "M0") << "|\n";
        std::cout << "| Sewa (2 rumah)    : " << std::left << std::setw(11) << (rents.size() > 2 ? formatUang(rents[2]) : "M0") << "|\n";
        std::cout << "| Sewa (3 rumah)    : " << std::left << std::setw(11) << (rents.size() > 3 ? formatUang(rents[3]) : "M0") << "|\n";
        std::cout << "| Sewa (4 rumah)    : " << std::left << std::setw(11) << (rents.size() > 4 ? formatUang(rents[4]) : "M0") << "|\n";
        std::cout << "| Sewa (hotel)      : " << std::left << std::setw(11) << (rents.size() > 5 ? formatUang(rents[5]) : "M0") << "|\n";
        std::cout << "+--------------------------------+\n";
        std::cout << "| Harga Rumah       : " << std::left << std::setw(11) << formatUang(street->getHousePrice()) << "|\n";
        std::cout << "| Harga Hotel       : " << std::left << std::setw(11) << formatUang(street->getHotelPrice()) << "|\n";
        std::cout << "+================================+\n";
    } else if (dynamic_cast<Railroad*>(targetProp)) {
        std::cout << "| Sewa 1 Stasiun    : M25        |\n";
        std::cout << "| Sewa 2 Stasiun    : M50        |\n";
        std::cout << "| Sewa 3 Stasiun    : M100       |\n";
        std::cout << "| Sewa 4 Stasiun    : M200       |\n";
        std::cout << "+================================+\n";
    } else if (dynamic_cast<Utility*>(targetProp)) {
        std::cout << "| Sewa 1 Utilitas   : Dadu x 4   |\n";
        std::cout << "| Sewa 2 Utilitas   : Dadu x 10  |\n";
        std::cout << "+================================+\n";
    }
    
    Player* owner = targetProp->getOwner();
    std::string statusStr;
    if (owner == nullptr) {
        statusStr = "TIDAK DIMILIKI";
    } else {
        if (targetProp->getIsMortgaged()) {
            statusStr = "MORTGAGED (" + owner->getName() + ")";
        } else {
            statusStr = "OWNED (" + owner->getName() + ")";
        }
    }
    
    std::cout << "| Status : " << std::left << std::setw(22) << statusStr << "|\n";
    std::cout << "+================================+\n";
}

void GameManager::executePostRoll(Player& p, int roll1, int roll2, int& doublesCount, bool& endTurnFlag) {
    int rollTotal = roll1 + roll2;
    std::cout << "Hasil: " << roll1 << " + " << roll2 << " = " << rollTotal << "\n";

    bool isDouble = (roll1 == roll2);

    if (isDouble) {
        doublesCount++;
        if (doublesCount == 3) {
            std::cout << "[!] 3 Kali KEMBAR! Bidak terlempar ke Penjara!\n";
            p.setPosition(10);
            p.setStatus(PlayerStatus::IN_JAIL);
            endTurnFlag = true;
            return;
        }
    }

    if (board.empty()) {
        std::cerr << "CRITICAL ERROR: Board is not properly initialized.\n";
        endTurnFlag = true;
        return;
    }

    int previousPosition = p.getPosition();
    p.move(rollTotal, static_cast<int>(board.size()));
    
    std::cout << "Memajukan Bidak " << p.getName() << " sebanyak " << rollTotal << " petak...\n";
    
    if (p.getPosition() < previousPosition) {
        p.addMoney(200);
        std::cout << "[!] Melewati GO! Mendapat bonus $200.\n";
    }

    Petak* currentSpace = board[p.getPosition()].get();
    std::cout << "Bidak mendarat di: " << currentSpace->getName() << ".\n\n";

    // Let the space execute its native injak logic (base functionality)
    currentSpace->injak(p);

    // Core interactive logic
    if (auto prop = dynamic_cast<PetakProperti*>(currentSpace)) {
        if (prop->getOwner() == nullptr) {
            std::string colorName = prop->getColorCode();
            std::string fullColor = "DEFAULT";
            if (colorName == "CK") fullColor = "COKLAT";
            else if (colorName == "BM") fullColor = "BIRU MUDA";
            else if (colorName == "PK") fullColor = "MERAH MUDA";
            else if (colorName == "OR") fullColor = "ORANGE";
            else if (colorName == "MR") fullColor = "MERAH";
            else if (colorName == "KN") fullColor = "KUNING";
            else if (colorName == "HJ") fullColor = "HIJAU";
            else if (colorName == "BT") fullColor = "BIRU TUA";
            else if (colorName == "AB") fullColor = "ABU-ABU";

            std::string kode = prop->getShortName();

            if (auto street = dynamic_cast<Street*>(prop)) {
                std::cout << "Kamu mendarat di " << prop->getName() << " (" << kode << ")!\n";
                
                std::string titleRow = "[" + fullColor + "] " + prop->getName() + " (" + kode + ")";
                if (titleRow.length() > 30) titleRow = titleRow.substr(0, 27) + "...";
                
                std::cout << "+================================+\n";
                std::cout << "|  " << std::left << std::setw(30) << titleRow << "|\n";
                std::cout << "| Harga Beli    : " << std::left << std::setw(15) << formatUang(prop->getHargaBeli()) << "|\n";
                const auto& rents = street->getRentPrices();
                std::string sewaDasar = (rents.size() > 0) ? formatUang(rents[0]) : "M0";
                std::cout << "| Sewa dasar    : " << std::left << std::setw(15) << sewaDasar << "|\n";
                std::cout << "| ...                            |\n";
                std::cout << "+================================+\n";
                
                std::cout << "Uang kamu saat ini: " << formatUang(p.getMoney()) << "\n";
                std::cout << "Apakah kamu ingin membeli properti ini seharga " << formatUang(prop->getHargaBeli()) << "? (y/n): ";
                
                std::string choice;
                std::getline(std::cin, choice);

                if (choice == "Y" || choice == "y") {
                    if (p.getMoney() >= prop->getHargaBeli()) {
                        p.reduceMoney(prop->getHargaBeli());
                        prop->setOwner(&p);
                        p.addProperty(prop);
                        std::cout << prop->getName() << " kini menjadi milikmu!\n";
                        std::cout << "Uang tersisa: " << formatUang(p.getMoney()) << "\n";
                    } else {
                        std::cout << "Uang tidak cukup!\nProperti ini akan masuk ke sistem lelang...\n";
                    }
                } else {
                    std::cout << "Properti ini akan masuk ke sistem lelang...\n";
                }
            }
            else if (dynamic_cast<Railroad*>(prop)) {
                std::cout << "Kamu mendarat di " << prop->getName() << " (" << kode << ")!\n";
                prop->setOwner(&p);
                p.addProperty(prop);
                std::cout << "Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!\n";
            }
            else if (dynamic_cast<Utility*>(prop)) {
                std::cout << "Kamu mendarat di " << prop->getName() << " (" << kode << ")!\n";
                prop->setOwner(&p);
                p.addProperty(prop);
                std::cout << "Belum ada yang menginjaknya duluan, " << prop->getName() << " kini menjadi milikmu!\n";
            }
        } else if (prop->getOwner() != &p) {
            // Pembayaran sewa sudah otomatis ditangani secara mandiri
            // oleh currentSpace->injak(p) mengikuti prinsip Tell, Don't Ask!
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
    
    if (p.getMoney() < 0) {
        p.setStatus(PlayerStatus::BANKRUPT);
        std::cout << "!!! " << p.getName() << " WENT BANKRUPT !!!\n";
        endTurnFlag = true;
        return;
    }

    if (!isDouble) {
        endTurnFlag = true;
    }
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
    bool endTurnFlag = false;

    while (!endTurnFlag) {
        std::cout << "> Masukkan perintah: ";
        std::string command;
        std::getline(std::cin, command); 

        if (command == "CETAK_PAPAN") {
            BoardView::printBoard(board, players, turnCount);
            continue; 
        }

        std::stringstream ss(command);
        std::string action;
        ss >> action;

        if (action == "LEMPAR_DADU" || command == "") {
            std::cout << "Mengocok dadu...\n";
            dice.roll();
            auto rollValues = dice.getLastRoll();
            executePostRoll(p, rollValues.first, rollValues.second, doublesCount, endTurnFlag);
        }
        else if (action == "CETAK_AKTA") {
            cetakAktaCommand();
        }
        else if (action == "HELP") {
            std::cout << "============= DAFTAR PERINTAH =============\n";
            std::cout << "  CETAK_PAPAN   : Menampilkan peta/board\n";
            std::cout << "  LEMPAR_DADU   : Melempar dadu acak (Bisa langsung Enter)\n";
            std::cout << "  ATUR_DADU X Y : Melempar dadu dengan nilai manual\n";
            std::cout << "  CETAK_AKTA    : Melihat info properti (Misal: JKT)\n";
            std::cout << "  HELP          : Menampilkan (bantuan) daftar command yang tersedia\n";
            std::cout << "===========================================\n";
        }
        else if (action == "ATUR_DADU") {
            int x, y;
            if (ss >> x >> y && x > 0 && y > 0) {
                std::cout << "Dadu diatur secara manual.\n";
                executePostRoll(p, x, y, doublesCount, endTurnFlag);
            } else {
                std::cout << "[!] Format salah atau angka tidak valid. Gunakan: ATUR_DADU X Y (X dan Y > 0)\n";
            }
        }
        else {
            std::cout << "[!] Perintah tidak valid.\n";
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
