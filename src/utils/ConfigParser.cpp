#include "utils/ConfigParser.hpp"
#include "models/Street.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"
#include "models/PetakAksi.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

struct MockTile {
    int id; int type; std::string name; std::string shortName; std::string color;
};

std::vector<std::unique_ptr<Petak>> ConfigParser::loadBoardConfig(const std::string& filename) {
    std::vector<std::unique_ptr<Petak>> board;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filename << ". Using NIMONSPOLI visual layout fallback.\n";
        
        MockTile layout[40] = {
            {0, 0, "GO", "GO", "DF"},
            {1, 1, "Garut", "GRT", "CK"},
            {2, 2, "Dana Umum", "DNU", "DF"},
            {3, 1, "Tasikmalaya", "TSK", "CK"},
            {4, 3, "Pajak Penghasilan", "PPH", "DF"},
            {5, 4, "Stasiun Gambir", "GBR", "DF"},
            {6, 1, "Bogor", "BGR", "BM"},
            {7, 2, "Kesempatan", "FES", "DF"},
            {8, 1, "Depok", "DPK", "BM"},
            {9, 1, "Bekasi", "BKS", "BM"},
            {10, 6, "Penjara", "PEN", "DF"},
            {11, 1, "Magelang", "MGL", "PK"},
            {12, 5, "PLN", "PLN", "AB"},
            {13, 1, "Solo", "SOL", "PK"},
            {14, 1, "Yogyakarta", "YOG", "PK"},
            {15, 4, "Stasiun Tugu", "STB", "DF"},
            {16, 1, "Malang", "MAL", "OR"},
            {17, 2, "Dana Umum", "DNU", "DF"},
            {18, 1, "Semarang", "SMG", "OR"},
            {19, 1, "Surabaya", "SBY", "OR"},
            {20, 7, "Bebas Parkir", "BBP", "DF"},
            {21, 1, "Makassar", "MKS", "MR"},
            {22, 2, "Kesempatan", "KSP", "DF"},
            {23, 1, "Balikpapan", "BLP", "MR"},
            {24, 1, "Manado", "MND", "MR"},
            {25, 4, "Stasiun Gubeng", "TUG", "DF"},
            {26, 1, "Palembang", "PLB", "KN"},
            {27, 1, "Pekanbaru", "PKB", "KN"},
            {28, 5, "PAM", "PAM", "AB"},
            {29, 1, "Medan", "MED", "KN"},
            {30, 8, "Pergi ke Penjara", "PPJ", "DF"},
            {31, 1, "Bandung", "BDG", "HJ"},
            {32, 1, "Denpasar", "DEN", "HJ"},
            {33, 2, "Kesempatan", "FES", "DF"},
            {34, 1, "Mataram", "MTR", "HJ"},
            {35, 4, "Stasiun Pasar Turi", "GUB", "DF"},
            {36, 2, "Kesempatan", "KSP", "DF"},
            {37, 1, "Jakarta", "JKT", "BT"},
            {38, 3, "Pajak Bumi", "PBM", "DF"},
            {39, 1, "IKN", "IKN", "BT"}
        };

        for(int i = 0; i < 40; i++) {
            std::unique_ptr<Petak> p;
            auto t = layout[i];
            
            if (t.type == 0) p = std::make_unique<PetakAksi>(t.id, t.name, ActionType::GO, 200);
            else if (t.type == 1) p = std::make_unique<Street>(t.id, t.name, t.color, 100 + i*10, std::vector<int>{10+i, 40+i*2, 100+i*3, 300+i*4, 500+i*5, 800+i*6}, 50, 50);
            else if (t.type == 2) p = std::make_unique<PetakAksi>(t.id, t.name, ActionType::FREE_PARKING);
            else if (t.type == 3) p = std::make_unique<PetakAksi>(t.id, t.name, ActionType::TAX, 200);
            else if (t.type == 4) p = std::make_unique<Railroad>(t.id, t.name, 200, 25);
            else if (t.type == 5) p = std::make_unique<Utility>(t.id, t.name, 150);
            else if (t.type == 6) p = std::make_unique<PetakAksi>(t.id, t.name, ActionType::FREE_PARKING);
            else if (t.type == 7) p = std::make_unique<PetakAksi>(t.id, t.name, ActionType::FREE_PARKING);
            else if (t.type == 8) p = std::make_unique<PetakAksi>(t.id, t.name, ActionType::JAIL);
            
            p->setSkin(t.shortName, t.color);
            board.push_back(std::move(p));
        }
        return board;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; 

        std::stringstream ss(line);
        std::string typeStr, idStr, name, currStr;
        
        std::getline(ss, typeStr, ',');
        std::getline(ss, idStr, ',');
        std::getline(ss, name, ',');
        
        int id = 0;
        try { id = std::stoi(idStr); } catch (...) {}
        
        std::unique_ptr<Petak> p;
        
        if (typeStr == "Street") {
            std::string colorGroupStr, rentBaseStr, housePriceStr, hotelPriceStr;
            std::getline(ss, colorGroupStr, ',');
            std::getline(ss, currStr, ',');
            std::getline(ss, rentBaseStr, ',');
            std::getline(ss, housePriceStr, ',');
            std::getline(ss, hotelPriceStr, ',');

            int hargaBeli = std::stoi(currStr);
            int baseRent = std::stoi(rentBaseStr);
            int housePrice = std::stoi(housePriceStr);
            int hotelPrice = hotelPriceStr.empty() ? housePrice : std::stoi(hotelPriceStr);

            std::vector<int> rentPrices = {baseRent, baseRent*5, baseRent*15, baseRent*40, baseRent*70, baseRent*100};
            p = std::make_unique<Street>(id, name, colorGroupStr, hargaBeli, rentPrices, housePrice, hotelPrice);
        } else if (typeStr == "Railroad") {
            std::getline(ss, currStr, ',');
            p = std::make_unique<Railroad>(id, name, std::stoi(currStr), 25);
        } else if (typeStr == "Utility") {
            std::getline(ss, currStr, ',');
            p = std::make_unique<Utility>(id, name, std::stoi(currStr));
        } else if (typeStr == "Action") {
            std::string actionTypeStr, amountStr;
            std::getline(ss, actionTypeStr, ',');
            std::getline(ss, amountStr, ',');
            
            int amount = amountStr.empty() ? 0 : std::stoi(amountStr);
            ActionType actionEnum = ActionType::FREE_PARKING;
            
            if (actionTypeStr == "GO") actionEnum = ActionType::GO;
            else if (actionTypeStr == "JAIL") actionEnum = ActionType::JAIL;
            else if (actionTypeStr == "TAX") actionEnum = ActionType::TAX;
            
            p = std::make_unique<PetakAksi>(id, name, actionEnum, amount);
        }

        if (p) {
            // Apply standard mask since missing CSV context
            p->setSkin("DEF", "DF");
            board.push_back(std::move(p));
        }
    }
    return board;
}
