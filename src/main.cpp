#include "core/GameManager.hpp"

#ifdef USE_RAYLIB
#include "views/RaylibUI.hpp"
#endif

#include <string>

int main(int argc, char *argv[]) {
  GameManager game;
  game.loadConfig("data/property.txt");

#ifdef USE_RAYLIB
  bool guiMode = (argc > 1 && std::string(argv[1]) == "--gui");
  if (guiMode) {
    game.setUI(std::make_unique<RaylibUI>(&game));
  }
#endif

  game.runInteractiveMenu();
  game.startGame();
  return 0;
}
