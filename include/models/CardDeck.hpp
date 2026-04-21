#ifndef CARDDECK_HPP
#define CARDDECK_HPP

#include "models/Card.hpp"
#include <vector>
#include <string>

class CardDeck {
private:
    std::string deckName;
    std::vector<Card> cards;

public:
    CardDeck(const std::string& name);
    
    std::string getName() const;
    void addCard(const Card& card);
    
    void shuffleDeck();
    Card drawCard();
    
    bool isEmpty() const;
};

#endif
