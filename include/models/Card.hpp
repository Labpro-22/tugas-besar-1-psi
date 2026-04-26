#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Player;

class Card {
protected:
  std::string description;

public:
  Card(const std::string &desc);
  virtual ~Card() = default;

  std::string getDescription() const;

  virtual void applyEffect(Player &p) = 0;
};

class MoneyCard : public Card {
private:
  int amount;

public:
  MoneyCard(const std::string &desc, int amount);
  void applyEffect(Player &p) override;
};

class MoveCard : public Card {
private:
  int targetPosition;

public:
  MoveCard(const std::string &desc, int targetPosition);
  void applyEffect(Player &p) override;
};

class JailCard : public Card {
public:
  JailCard(const std::string &desc);
  void applyEffect(Player &p) override;
};

enum class SpecialCardType {
  MOVE, DISCOUNT, SHIELD, TELEPORT, LASSO, DEMOLITION
};

class SpecialCard {
private:
  SpecialCardType cardType;
  int value;// langkah (MOVE) atau persen diskon (DISCOUNT)
  int duration; // sisa durasi berlaku (hanya DISCOUNT)

public:
  SpecialCard(SpecialCardType type, int value = 0, int duration = 0);

  SpecialCardType getType()     const;
  int             getValue()    const;
  int             getDuration() const;
  void            setDuration(int d);
  void            decrementDuration();

  std::string getTypeName()    const;
  std::string getDescription() const;
};

#endif
