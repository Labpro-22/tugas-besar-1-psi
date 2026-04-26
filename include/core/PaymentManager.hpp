#ifndef PAYMENTMANAGER_HPP
#define PAYMENTMANAGER_HPP

#include <vector>
#include <memory>
#include <string>

class Player;
class Petak;
class IGameUI;

class PaymentManager {
public:
  static void processPayment(Player &debtor, Player *creditor, int amount,
                             IGameUI &ui,
                             std::vector<std::unique_ptr<Petak>> &board,
                             std::vector<Player> &players,
                             const std::string &reason = "");
};

#endif
