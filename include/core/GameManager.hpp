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
    bool hasDoneAction;

    // Helper method for handling declined properties
    void startAuction(PetakProperti* prop, Player* initiator);
    void executePostRoll(Player& p, int roll1, int roll2, int& doublesCount, bool& endTurnFlag);
    void cetakAktaCommand();
    void cetakPropertiCommand(const std::string& playerName);
    void handleBangunCommand();
    void handleGadaiCommand();
    void handleTebusCommand();
    void handleSimpanCommand(const std::string& args);

public:
    GameManager();
    
    void loadConfig(const std::string& filename);
    void initPlayers(int numPlayers);
    void playTurn();
    void nextTurn();
    Player* checkWinner();
    void startGame();

    const std::vector<Player>& getPlayers() const { return players; }
    const std::vector<std::unique_ptr<Petak>>& getBoard() const { return board; }
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }
    int getTurnCount() const { return turnCount; }
};

#endif
