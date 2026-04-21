#include "views/BoardView.hpp"
#include "models/Petak.hpp"
#include "models/PetakProperti.hpp"
#include "models/Street.hpp"
#include "models/Player.hpp"
#include <iostream>
#include <string>

std::string getAnsiColor(const std::string& code) {
    if (code == "CK") return "\033[38;5;130m"; // Brown
    if (code == "MR") return "\033[91m";       // Red
    if (code == "BM") return "\033[96m";       // Light Cyan
    if (code == "KN") return "\033[93m";       // Light Yellow
    if (code == "PK") return "\033[95m";       // Pink
    if (code == "HJ") return "\033[92m";       // Green
    if (code == "OR") return "\033[38;5;208m"; // Orange
    if (code == "BT") return "\033[94m";       // Blue
    if (code == "AB") return "\033[90m";       // Light Gray
    return "\033[0m"; // Default DF
}

std::pair<std::string, std::string> getCellStrings(int pos, const std::vector<std::unique_ptr<Petak>>& board, const std::vector<Player>& players) {
    if (pos < 0 || pos >= static_cast<int>(board.size())) return {"          ", "          "};
    
    Petak* p = board[pos].get();
    std::string colorCode = p->getColorCode();
    std::string shortName = p->getShortName();

    std::string ansi = getAnsiColor(colorCode);
    std::string reset = "\033[0m";

    std::string line1 = " [" + colorCode + "] " + shortName;
    while (line1.length() < 10) line1 += " ";
    std::string visualLine1 = ansi + line1 + reset;
    
    std::string line2 = "";
    if (auto prop = dynamic_cast<PetakProperti*>(p)) {
        if (Player* owner = prop->getOwner()) {
            line2 += "P" + std::to_string(owner->getId()) + " ";
            if (auto str = dynamic_cast<Street*>(prop)) {
                int hc = str->getHouseCount();
                if (hc == 5) line2 += "* ";
                else {
                    for (int i = 0; i < hc; i++) line2 += "^";
                }
            }
        }
    }
    
    std::string tokens = "";
    for (const auto& player : players) {
        if (player.getPosition() == pos) {
            tokens += std::to_string(player.getId());
        }
    }
    
    if (!tokens.empty()) {
        if (line2.length() > 0) line2 += " ";
        line2 += "(" + tokens + ")";
    }
    
    while (line2.length() < 10) line2 += " ";
    if (line2.length() > 10) line2 = line2.substr(0, 10);
    
    return {visualLine1, ansi + line2 + reset};
}

void BoardView::printBoard(const std::vector<std::unique_ptr<Petak>>& board, const std::vector<Player>& players, int turnCount) {
    if (board.size() < 40) return;

    auto printHorizBorderFull = []() {
        for(int i=0; i<11; i++) std::cout << "+----------";
        std::cout << "+\n";
    };

    std::cout << "\n> CETAK_PAPAN\n\n";

    printHorizBorderFull();
    for (int line = 1; line <= 2; line++) {
        for (int i = 20; i <= 30; i++) {
            std::cout << "|";
            auto strs = getCellStrings(i, board, players);
            std::cout << (line == 1 ? strs.first : strs.second);
        }
        std::cout << "|\n";
    }
    printHorizBorderFull();

    std::vector<std::string> centerStr(18, std::string(98, ' '));
    auto putText = [&](int row, int col, const std::string& txt) {
        for (size_t i = 0; i < txt.length() && (col + i) < centerStr[row].length(); i++) {
            centerStr[row][col + i] = txt[i];
        }
    };
    
    putText(2, 29, "========================================");
    putText(3, 29, "||             NIMONSPOLI             ||");
    putText(4, 29, "========================================");
    
    putText(6, 40, "TURN " + std::to_string(turnCount) + " / 50");
    
    putText(8, 29, "--------------------------------------------------");
    putText(9, 29, "LEGENDA KEPEMILIKAN & STATUS");
    putText(10, 29, "P1-P4 : Properti milik Pemain 1-4");
    putText(11, 29, "^     : Rumah Level 1");
    putText(12, 29, "^^    : Rumah Level 2");
    putText(13, 29, "^^^   : Rumah Level 3");
    putText(14, 29, "*     : Hotel (Maksimal)");
    putText(15, 29, "(1)-(4) : Bidak (Pemasisi Pemain saat ini)");
    putText(16, 29, "--------------------------------------------------");

    for (int r = 1; r <= 9; r++) {
        int leftPos = 20 - r; 
        int rightPos = 30 + r; 
        
        auto leftStrs = getCellStrings(leftPos, board, players);
        auto rightStrs = getCellStrings(rightPos, board, players);
        
        std::cout << "|" << leftStrs.first << "|" << centerStr[(r-1)*2] << "|" << rightStrs.first << "|\n";
        std::cout << "|" << leftStrs.second << "|" << centerStr[(r-1)*2 + 1] << "|" << rightStrs.second << "|\n";
        
        if (r < 9) {
            std::cout << "+----------+" << std::string(98, ' ') << "+----------+\n";
        }
    }
    
    printHorizBorderFull();
    for (int line = 1; line <= 2; line++) {
        for (int i = 10; i >= 0; i--) {
            std::cout << "|";
            auto strs = getCellStrings(i, board, players);
            std::cout << (line == 1 ? strs.first : strs.second);
        }
        std::cout << "|\n";
    }
    printHorizBorderFull();
}
