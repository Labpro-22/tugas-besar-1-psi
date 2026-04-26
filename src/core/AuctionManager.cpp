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

    std::string action;
    int bidAmount = 0;

    if (currentP->isComPlayer()) {
      ui.showMessage("Giliran: " + currentP->getName() + " [COM]");

      int maxWilling = static_cast<int>(prop->getHargaBeli() * 0.8);
      int maxAffordable = currentP->getMoney();
      int maxBid = std::min(maxWilling, maxAffordable);

      if (highestBid < maxBid) {
        bidAmount = highestBid + std::max(50, prop->getHargaBeli() / 10);
        if (bidAmount <= maxBid && bidAmount > 0) {
          action = "BID";
          ui.showMessage("[COM " + currentP->getName() + "] BID " +
                         formatUang(bidAmount) +
                         " (strategi: maks 80% harga properti)");
        } else {
          action = "PASS";
          ui.showMessage("[COM " + currentP->getName() + "] PASS (bid melebihi batas)");
        }
      } else {
        action = "PASS";
        ui.showMessage("[COM " + currentP->getName() + "] PASS (harga sudah terlalu tinggi)");
      }
    } else {

      ui.showMessage("Giliran: " + currentP->getName());
      std::string input = ui.promptInput("Aksi (PASS / BID <jumlah>):\n> ");

      std::stringstream ss(input);
      ss >> action;
      if (action == "BID") {
        if (!(ss >> bidAmount)) {
          ui.showMessage("Format BID salah!\n");
          continue;
        }
      }
    }

    if (action == "BID") {
      if (bidAmount > currentP->getMoney()) {
        ui.showMessage("Uang tidak cukup untuk BID!\n");
        continue;
      }
      if (highestBid > 0 && bidAmount <= highestBid) {
        ui.showMessage("BID harus lebih besar dari penawaran tertinggi (" +
                       formatUang(highestBid) + ")!\n");
        continue;
      }
      if (highestBid == 0 && bidAmount <= 0) {
        ui.showMessage("BID harus lebih besar dari 0!\n");
        continue;
      }
      highestBid = bidAmount;
      highestBidder = currentP;
      consecutivePass = 0;
      ui.showMessage("Penawaran tertinggi: " + formatUang(highestBid) + " (" +
                     highestBidder->getName() + ")\n");
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
