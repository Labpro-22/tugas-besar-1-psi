#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP

#include <string>
#include <vector>

class GameManager;

class SaveManager {
public:
  static bool saveGame(const GameManager &gm, const std::string &filename);
  static bool loadGame(GameManager &gm, const std::string &filename);
};

#endif
