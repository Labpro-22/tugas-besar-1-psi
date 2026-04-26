#include "core/ComPlayerAI.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Street.hpp"
#include "models/Card.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"

#include <algorithm>
#include <chrono>

ComPlayerAI::ComPlayerAI()
    : rng(static_cast<unsigned>(
          std::chrono::steady_clock::now().time_since_epoch().count())) {}

bool ComPlayerAI::decide(double probability) {
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(rng) < probability;
}

std::string ComPlayerAI::comPrefix(const Player &p) {
  return "[" + p.getName() + "] ";
}

void ComPlayerAI::logDecision(const Player &p, IGameUI &ui,
                              const std::string &action,
                              const std::string &reasoning) {
  std::string msg = comPrefix(p) + action;
  if (!reasoning.empty())
    msg += " (" + reasoning + ")";
  ui.showMessage(msg);
}

bool ComPlayerAI::shouldBuyProperty(const Player &p,
                                    const PetakProperti *prop) {
  int price = prop->getHargaBeli();
  int money = p.getMoney();

  if (money < price)
    return false;

  if (money > price * 2)
    return decide(0.70);

  if (money > price * 3 / 2)
    return decide(0.50);

  return decide(0.30);
}

int ComPlayerAI::chooseTaxOption(const Player & , int flatAmount,
                                 int percentageAmount) {

  return (flatAmount <= percentageAmount) ? 1 : 2;
}

int ComPlayerAI::decideAuctionBid(const Player &p, const PetakProperti *prop,
                                  int currentHighestBid) {
  int propertyValue = prop->getHargaBeli();
  int maxWillingToPay = static_cast<int>(propertyValue * 0.8);
  int money = p.getMoney();

  int maxBid = std::min(maxWillingToPay, money);

  if (currentHighestBid >= maxBid)
    return 0;

  int bid = currentHighestBid + std::max(50, propertyValue / 10);
  if (bid > maxBid)
    return 0;

  if (!decide(0.60))
    return 0;

  return bid;
}

int ComPlayerAI::chooseLiquidationOption(const Player & , int numOptions) {

  if (numOptions > 0)
    return 1;
  return 0;
}

std::string
ComPlayerAI::chooseFestivalProperty(const Player & ,
                                    const std::vector<Street *> &options) {
  if (options.empty())
    return "";

  Street *best = options[0];
  int bestRent = best->getSewa();
  for (size_t i = 1; i < options.size(); ++i) {
    int rent = options[i]->getSewa();
    if (rent > bestRent) {
      bestRent = rent;
      best = options[i];
    }
  }
  return best->getShortName();
}

int ComPlayerAI::chooseCardToDrop(const Player &p) {

  const auto &hand = p.getHand();
  for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
    if (hand[i]->getType() == SpecialCardType::MOVE)
      return i + 1;
  }
  return 1;
}

int ComPlayerAI::chooseSpecialCardToUse(const Player &p) {
  const auto &hand = p.getHand();
  if (hand.empty())
    return 0;

  for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
    if (hand[i]->getType() == SpecialCardType::SHIELD) {

      if (decide(0.40))
        return i + 1;
    }
  }

  for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
    if (hand[i]->getType() == SpecialCardType::DISCOUNT) {

      if (decide(0.30))
        return i + 1;
    }
  }

  return 0;
}
