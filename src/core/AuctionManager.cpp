#include "core/AuctionManager.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "utils/FormatHelper.hpp"
#include "views/IGameUI.hpp"
#include <sstream>

void AuctionManager::startAuction(PetakProperti *prop, Player *initiator,
                                  std::vector<Player> &allPlayers,
                                  IGameUI &ui) {
  ui.showMessage("\nProperti " + prop->getName() + " (" + prop->getShortName() +
                 ") akan dilelang!");
  ui.showMessage("Urutan lelang dimulai dari pemain setelah " +
                 initiator->getName() + ".\n");

  std::vector<Player *> activeBidders;
  int initiatorIdx = -1;
  for (size_t i = 0; i < allPlayers.size(); i++) {
    if (allPlayers[i].getStatus() != PlayerStatus::BANKRUPT) {
      activeBidders.push_back(&allPlayers[i]);
      if (&allPlayers[i] == initiator)
        initiatorIdx = (int)activeBidders.size() - 1;
    }
  }

  if (activeBidders.empty())
    return;

  int currentBidderIdx = (initiatorIdx + 1) % activeBidders.size();
  int highestBid = 0;
  Player *highestBidder = nullptr;
  int consecutivePass = 0;

  while (activeBidders.size() > 1 &&
         consecutivePass < (int)activeBidders.size()) {
    Player *currentP = activeBidders[currentBidderIdx];

    ui.showMessage("Giliran: " + currentP->getName());
    std::string input = ui.promptInput("Aksi (PASS / BID <jumlah>):\n> ");

    std::stringstream ss(input);
    std::string action;
    ss >> action;

    if (action == "BID") {
      int amount;
      if (ss >> amount) {
        if (amount > currentP->getMoney()) {
          ui.showMessage("Uang tidak cukup untuk BID!\n");
          continue;
        }
        if (highestBid > 0 && amount <= highestBid) {
          ui.showMessage("BID harus lebih besar dari penawaran tertinggi (" +
                         formatUang(highestBid) + ")!\n");
          continue;
        }
        if (highestBid == 0 && amount <= 0) {
          ui.showMessage("BID harus lebih besar dari 0!\n");
          continue;
        }
        highestBid = amount;
        highestBidder = currentP;
        consecutivePass = 0;
        ui.showMessage("Penawaran tertinggi: " + formatUang(highestBid) + " (" +
                       highestBidder->getName() + ")\n");
      } else {
        ui.showMessage("Format BID salah!\n");
        continue;
      }
    } else {
      ui.showMessage(currentP->getName() + " PASS.\n");
      consecutivePass++;
    }

    currentBidderIdx = (currentBidderIdx + 1) % activeBidders.size();
  }

  ui.showMessage("Lelang selesai!");
  if (highestBidder) {
    ui.showMessage("Pemenang: " + highestBidder->getName());
    ui.showMessage("Harga akhir: " + formatUang(highestBid));
    ui.showMessage("Properti " + prop->getName() + " (" + prop->getShortName() +
                   ") kini dimiliki " + highestBidder->getName() + ".");

    highestBidder->reduceMoney(highestBid);
    prop->setOwner(highestBidder);
    highestBidder->addProperty(prop);
  } else {
    ui.showMessage("Tidak ada yang menawar. Properti tetap tidak dimiliki.");
  }
}
