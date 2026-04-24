#ifndef CARDDECK_HPP
#define CARDDECK_HPP

#include "models/Card.hpp"
#include <memory>
#include <string>
#include <vector>

class CardDeck {
private:
  std::string deckName;
  std::vector<std::unique_ptr<Card>> cards;

public:
  CardDeck(const std::string &name);

  std::string getName() const;
  void addCard(std::unique_ptr<Card> card);

  void shuffleDeck();
  std::unique_ptr<Card> drawCard();
  void returnCard(std::unique_ptr<Card> card);

  bool isEmpty() const;
};

#endif
