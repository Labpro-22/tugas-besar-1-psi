#include "models/CardDeck.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>

CardDeck::CardDeck(const std::string& name) : deckName(name) {}

std::string CardDeck::getName() const { return deckName; }

void CardDeck::addCard(const Card& card) {
    cards.push_back(card);
}

void CardDeck::shuffleDeck() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

Card CardDeck::drawCard() {
    if (cards.empty()) {
        throw std::out_of_range("Cannot draw from an empty CardDeck.");
    }
    
    // Draw from the top (front)
    Card drawn = cards.front();
    
    // Remove it from the front
    cards.erase(cards.begin());
    
    // Place it at the bottom (back)
    cards.push_back(drawn);
    
    return drawn;
}

bool CardDeck::isEmpty() const {
    return cards.empty();
}
