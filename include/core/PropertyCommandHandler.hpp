#ifndef PROPERTYCOMMANDHANDLER_HPP
#define PROPERTYCOMMANDHANDLER_HPP

#include <memory>
#include <string>
#include <vector>

class Petak;
class Player;
class IGameUI;

class PropertyCommandHandler {
public:
  static void cetakAkta(const std::vector<std::unique_ptr<Petak>> &board,
                        IGameUI &ui);

  static void cetakProperti(const std::vector<Player> &players, IGameUI &ui,
                            const std::string &playerName);

  static void handleBangun(const std::vector<std::unique_ptr<Petak>> &board,
                           Player &player, IGameUI &ui);

  static void handleGadai(const std::vector<std::unique_ptr<Petak>> &board,
                          Player &player, IGameUI &ui);

  static void handleTebus(Player &player, IGameUI &ui);
};

#endif
