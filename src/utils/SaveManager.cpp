#include "utils/SaveManager.hpp"
#include "core/GameManager.hpp"
#include "models/PetakProperti.hpp"
#include "models/Street.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// ─────────────────── SAVE ───────────────────

bool SaveManager::saveGame(const GameManager& gm, const std::string& filename) {
    // Cek file sudah ada
    {
        std::ifstream check(filename);
        if (check.good()) {
            check.close();
            std::cout << "File \"" << filename << "\" sudah ada. Timpa file lama? (y/n): ";
            std::string ans;
            std::getline(std::cin, ans);
            if (ans != "y" && ans != "Y") {
                std::cout << "Simpan dibatalkan.\n";
                return false;
            }
        }
    }

    std::ofstream out(filename);
    if (!out) {
        std::cout << "Gagal menyimpan file! Pastikan direktori dapat ditulis.\n";
        return false;
    }

    // Baris 1: TURN_SAAT_INI MAX_TURN
    out << gm.turnCount << " " << gm.maxTurn << "\n";

    // Baris 2: JUMLAH_PEMAIN
    out << gm.players.size() << "\n";

    // State tiap pemain
    for (const auto& p : gm.players) {
        std::string statusStr;
        switch (p.getStatus()) {
            case PlayerStatus::ACTIVE:   statusStr = "ACTIVE";   break;
            case PlayerStatus::IN_JAIL:  statusStr = "JAILED";   break;
            case PlayerStatus::BANKRUPT: statusStr = "BANKRUPT"; break;
        }
        out << p.getName() << " " << p.getMoney() << " "
            << p.getPosition() << " " << statusStr << "\n";
        out << "0\n"; // JUMLAH_KARTU_TANGAN (belum diimplementasikan)
    }

    // Urutan giliran (nama pemain berurutan)
    for (size_t i = 0; i < gm.players.size(); i++) {
        if (i > 0) out << " ";
        out << gm.players[i].getName();
    }
    out << "\n";

    // Giliran aktif saat ini
    out << gm.players[gm.currentPlayerIndex].getName() << "\n";

    // State properti
    int propCount = 0;
    for (const auto& petak : gm.board) {
        if (dynamic_cast<PetakProperti*>(petak.get())) propCount++;
    }
    out << propCount << "\n";

    for (const auto& petak : gm.board) {
        const auto* prop = dynamic_cast<const PetakProperti*>(petak.get());
        if (!prop) continue;

        std::string jenis;
        if      (dynamic_cast<const Street*>(prop))   jenis = "street";
        else if (dynamic_cast<const Railroad*>(prop)) jenis = "railroad";
        else                                          jenis = "utility";

        std::string pemilik  = "BANK";
        std::string statusStr = "BANK";
        if (prop->getOwner()) {
            pemilik   = prop->getOwner()->getName();
            statusStr = prop->getIsMortgaged() ? "MORTGAGED" : "OWNED";
        }

        int fmult = 1, fdur = 0;
        std::string nBangunan = "0";

        if (auto* s = dynamic_cast<const Street*>(prop)) {
            fmult = s->getFestivalMultiplier();
            fdur  = s->getFestivalDuration();
            nBangunan = s->getIsHotel() ? "H" : std::to_string(s->getHouseCount());
        }

        out << petak->getShortName() << " " << jenis << " " << pemilik << " "
            << statusStr << " " << fmult << " " << fdur << " " << nBangunan << "\n";
    }

    // State deck kemampuan (belum diimplementasikan)
    out << "0\n";

    // State log (belum diimplementasikan)
    out << "0\n";

    out.close();
    std::cout << "Permainan berhasil disimpan ke: " << filename << "\n";
    return true;
}

// ─────────────────── LOAD ───────────────────

bool SaveManager::loadGame(GameManager& gm, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cout << "File \"" << filename << "\" tidak ditemukan.\n";
        return false;
    }

    try {
        std::string line;

        // Baris 1: turnCount maxTurn
        std::getline(in, line);
        {
            std::istringstream ss(line);
            ss >> gm.turnCount >> gm.maxTurn;
        }

        // Baris 2: jumlah pemain
        std::getline(in, line);
        int numPlayers = std::stoi(line);

        // Baca state pemain
        gm.players.clear();
        gm.players.reserve(numPlayers);

        for (int i = 0; i < numPlayers; i++) {
            std::getline(in, line);
            std::istringstream ss(line);
            std::string name, statusStr;
            int money, position;
            ss >> name >> money >> position >> statusStr;

            gm.players.emplace_back(i + 1, name, money);
            gm.players.back().setPosition(position);
            if      (statusStr == "JAILED")   gm.players.back().setStatus(PlayerStatus::IN_JAIL);
            else if (statusStr == "BANKRUPT") gm.players.back().setStatus(PlayerStatus::BANKRUPT);

            // skip kartu tangan
            std::getline(in, line);
            int cardCount = 0;
            try { cardCount = std::stoi(line); } catch (...) {}
            for (int j = 0; j < cardCount; j++) std::getline(in, line);
        }

        // Urutan giliran (skip — kita pakai urutan players[] apa adanya)
        std::getline(in, line);

        // Giliran aktif
        std::getline(in, line);
        {
            std::string activeName = line;
            gm.currentPlayerIndex = 0;
            for (int i = 0; i < (int)gm.players.size(); i++) {
                if (gm.players[i].getName() == activeName) {
                    gm.currentPlayerIndex = i;
                    break;
                }
            }
        }

        // Reset semua status properti di board
        for (auto& petak : gm.board) {
            if (auto* prop = dynamic_cast<PetakProperti*>(petak.get())) {
                prop->setOwner(nullptr);
                prop->setMortgaged(false);
                if (auto* s = dynamic_cast<Street*>(prop)) {
                    s->restoreBuildingState(0, false);
                    s->setFestivalState(1, 0);
                }
            }
        }

        // Baca state properti
        std::getline(in, line);
        int propCount = std::stoi(line);

        for (int i = 0; i < propCount; i++) {
            std::getline(in, line);
            std::istringstream ss(line);
            std::string kode, jenis, pemilik, statusStr, nBangunan;
            int fmult, fdur;
            ss >> kode >> jenis >> pemilik >> statusStr >> fmult >> fdur >> nBangunan;

            // Cari tile dengan kode ini
            PetakProperti* prop = nullptr;
            for (auto& petak : gm.board) {
                if (petak && petak->getShortName() == kode) {
                    prop = dynamic_cast<PetakProperti*>(petak.get());
                    if (prop) break;
                }
            }
            if (!prop) continue;

            if (pemilik != "BANK") {
                for (auto& p : gm.players) {
                    if (p.getName() == pemilik) {
                        prop->setOwner(&p);
                        p.addProperty(prop);
                        break;
                    }
                }
            }

            if (statusStr == "MORTGAGED") prop->setMortgaged(true);

            if (auto* s = dynamic_cast<Street*>(prop)) {
                s->setFestivalState(fmult, fdur);
                if (!nBangunan.empty() && nBangunan != "0") {
                    if (nBangunan == "H") {
                        s->restoreBuildingState(4, true);
                    } else {
                        try { s->restoreBuildingState(std::stoi(nBangunan), false); }
                        catch (...) {}
                    }
                }
            }
        }

        // Skip deck dan log
        std::getline(in, line); // jumlah kartu deck
        int deckCount = 0;
        try { deckCount = std::stoi(line); } catch (...) {}
        for (int i = 0; i < deckCount; i++) std::getline(in, line);

        std::getline(in, line); // jumlah log
        int logCount = 0;
        try { logCount = std::stoi(line); } catch (...) {}
        for (int i = 0; i < logCount; i++) std::getline(in, line);

        std::cout << "Permainan berhasil dimuat. Melanjutkan giliran "
                  << gm.players[gm.currentPlayerIndex].getName() << "...\n";
        return true;

    } catch (const std::exception& e) {
        std::cout << "Gagal memuat file! File rusak atau format tidak dikenali.\n";
        return false;
    }
}
