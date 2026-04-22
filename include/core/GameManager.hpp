#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <vector>
#include <string>
#include <memory>

#include "models/Player.hpp"
#include "models/Petak.hpp"
#include "models/Dice.hpp"
#include "models/CardDeck.hpp"

class PetakProperti;

class GameManager {
private:
    std::vector<Player> players;
    std::vector<std::unique_ptr<Petak>> board;
    Dice dice;
    CardDeck chanceDeck;
    CardDeck chestDeck;
    
    int currentPlayerIndex;
    int turnCount;

    // Helper method for handling declined properties
    void startAuction(PetakProperti* prop);
    void executePostRoll(Player& p, int roll1, int roll2, int& doublesCount, bool& endTurnFlag);
    void cetakAktaCommand();
    void handleBangunCommand(const std::string& kode);

public:
    GameManager();
    
    void loadConfig(const std::string& filename);
    void initPlayers(int numPlayers);
    void playTurn();
    void nextTurn();
    Player* checkWinner();
    void startGame();
};

#endif
