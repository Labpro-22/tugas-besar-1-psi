#include "models/CardDeck.hpp"
#include <algorithm>
#include <random>
#include <stdexcept>

CardDeck::CardDeck(const std::string &name) : deckName(name) {}

std::string CardDeck::getName() const { return deckName; }

void CardDeck::addCard(std::unique_ptr<Card> card) {
  cards.push_back(std::move(card));
}

void CardDeck::shuffleDeck() {
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(cards.begin(), cards.end(), g);
}

std::unique_ptr<Card> CardDeck::drawCard() {
  if (cards.empty()) {
    throw std::out_of_range("Cannot draw from an empty CardDeck.");
  }

  std::unique_ptr<Card> drawn = std::move(cards.front());
  cards.erase(cards.begin());

  return drawn;
}

void CardDeck::returnCard(std::unique_ptr<Card> card) {
  cards.push_back(std::move(card));
}

bool CardDeck::isEmpty() const { return cards.empty(); }
