#ifndef COMPLAYERAI_HPP
#define COMPLAYERAI_HPP

#include <memory>
#include <random>
#include <string>
#include <vector>

class Player;
class PetakProperti;
class Street;
class IGameUI;
class Petak;

class ComPlayerAI {
private:
  std::mt19937 rng;

  bool decide(double probability);

  static std::string comPrefix(const Player &p);

public:
  ComPlayerAI();

  bool shouldBuyProperty(const Player &p, const PetakProperti *prop);

  int chooseTaxOption(const Player &p, int flatAmount, int percentageAmount);

  int decideAuctionBid(const Player &p, const PetakProperti *prop,
                       int currentHighestBid);

  int chooseLiquidationOption(const Player &p, int numOptions);

  std::string chooseFestivalProperty(const Player &p,
                                     const std::vector<Street *> &options);

  int chooseCardToDrop(const Player &p);

  int chooseSpecialCardToUse(const Player &p);

  static void logDecision(const Player &p, IGameUI &ui,
                          const std::string &action,
                          const std::string &reasoning = "");
};

#endif
