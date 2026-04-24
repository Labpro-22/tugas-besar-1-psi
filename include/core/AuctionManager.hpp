#ifndef AUCTIONMANAGER_HPP
#define AUCTIONMANAGER_HPP

#include <vector>

class Player;
class PetakProperti;
class IGameUI;

class AuctionManager {
public:
  static void startAuction(PetakProperti *prop, Player *initiator,
                           std::vector<Player> &allPlayers, IGameUI &ui);
};

#endif
