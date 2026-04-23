#include "models/PetakAksi.hpp"
#include "models/Player.hpp"
#include "models/PetakProperti.hpp"
#include "models/Street.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"
#include <iostream>
#include <iomanip>
#include <string>

static std::string formatUangLocal(int amount) {
    std::string s = std::to_string(amount);
    int n = s.length();
    for (int i = n - 3; i > 0; i -= 3) {
        s.insert(i, ".");
    }
    return "M" + s;
}

PetakAksi::PetakAksi(int id, const std::string& name, ActionType type, int amount)
    : Petak(id, name), type(type), amount(amount) {}

ActionType PetakAksi::getType() const { return type; }

int PetakAksi::getAmount() const { return amount; }

void PetakAksi::injak(Player& p, int /*diceRoll*/) {
    switch (type) {
        case ActionType::GO:
            p.addMoney(amount > 0 ? amount : 200);
            break;
        case ActionType::JAIL:
            p.setStatus(PlayerStatus::IN_JAIL);
            p.setPosition(10); 
            break;
        case ActionType::TAX: {
            if (getShortName() == "PPH") {
                std::cout << "Kamu mendarat di Pajak Penghasilan (PPH)!\n";
                std::cout << "Pilih opsi pembayaran pajak:\n";
                std::cout << "1. Bayar flat " << formatUangLocal(amount) << "\n";
                std::cout << "2. Bayar 10% dari total kekayaan\n";
                std::cout << "(Pilih sebelum menghitung kekayaan!)\n";
                
                int pilihan = 0;
                while (true) {
                    std::cout << "Pilihan (1/2): ";
                    std::string input;
                    if (!std::getline(std::cin, input)) break; 
                    try {
                        pilihan = std::stoi(input);
                        if (pilihan == 1 || pilihan == 2) break;
                    } catch (...) {}
                    std::cout << "Input tidak valid. Masukkan 1 atau 2.\n";
                }
                
                std::cout << "\n";
                if (pilihan == 1) {
                    if (p.getMoney() >= amount) {
                        int pMoneyBefore = p.getMoney();
                        p.reduceMoney(amount);
                        std::cout << "Uang kamu: " << formatUangLocal(pMoneyBefore) << " -> " << formatUangLocal(p.getMoney()) << "\n";
                    } else {
                        std::cout << "Kamu tidak mampu membayar pajak flat " << formatUangLocal(amount) << "!\n";
                        std::cout << "Uang kamu saat ini: " << formatUangLocal(p.getMoney()) << "\n";
                        std::cout << "// Alur dilanjutkan ke Kebangkrutan\n";
                        p.reduceMoney(amount); 
                    }
                } else {
                    int cash = p.getMoney();
                    int totalPropValue = 0;
                    int totalBldgValue = 0;
                    
                    for (auto* prop : p.getOwnedProperties()) {
                        totalPropValue += prop->getHargaBeli();
                        if (auto street = dynamic_cast<Street*>(prop)) {
                            totalBldgValue += street->getTotalBuildingCost();
                        }
                    }
                    
                    int totalKekayaan = cash + totalPropValue + totalBldgValue;
                    int pajak10 = totalKekayaan / 10;
                    
                    std::cout << "Total kekayaan kamu:\n";
                    std::cout << "- Uang tunai            : " << formatUangLocal(cash) << "\n";
                    std::cout << "- Harga beli properti   : " << formatUangLocal(totalPropValue) << " (termasuk yang digadaikan)\n";
                    std::cout << "- Harga beli bangunan   : " << formatUangLocal(totalBldgValue) << "\n";
                    std::cout << "Total                   : " << formatUangLocal(totalKekayaan) << "\n";
                    std::cout << "Pajak 10%               : " << formatUangLocal(pajak10) << "\n";
                    
                    if (p.getMoney() >= pajak10) {
                        int pMoneyBefore = p.getMoney();
                        p.reduceMoney(pajak10);
                        std::cout << "Uang kamu: " << formatUangLocal(pMoneyBefore) << " -> " << formatUangLocal(p.getMoney()) << "\n";
                    } else {
                        std::cout << "Kamu tidak mampu membayar pajak 10% (" << formatUangLocal(pajak10) << ")!\n";
                        std::cout << "Uang kamu saat ini: " << formatUangLocal(p.getMoney()) << "\n";
                        std::cout << "// Alur dilanjutkan ke Kebangkrutan\n";
                        p.reduceMoney(pajak10);
                    }
                }
            } else if (getShortName() == "PBM") {
                std::cout << "Kamu mendarat di Pajak Barang Mewah (PBM)!\n";
                std::cout << "Pajak sebesar " << formatUangLocal(amount) << " langsung dipotong.\n";
                if (p.getMoney() >= amount) {
                    int pMoneyBefore = p.getMoney();
                    p.reduceMoney(amount);
                    std::cout << "Uang kamu: " << formatUangLocal(pMoneyBefore) << " -> " << formatUangLocal(p.getMoney()) << "\n";
                } else {
                    std::cout << "Kamu tidak mampu membayar pajak!\n";
                    std::cout << "Uang kamu saat ini: " << formatUangLocal(p.getMoney()) << "\n";
                    std::cout << "// Alur dilanjutkan ke Kebangkrutan\n";
                    p.reduceMoney(amount);
                }
            } else {
                p.reduceMoney(amount);
            }
            break;
        }
        case ActionType::FREE_PARKING:
            break;

        case ActionType::FESTIVAL: {
            std::cout << "Kamu mendarat di petak Festival!\n";

            std::vector<Street*> ownedStreets;
            for (auto* prop : p.getOwnedProperties()) {
                if (auto* s = dynamic_cast<Street*>(prop)) {
                    ownedStreets.push_back(s);
                }
            }

            if (ownedStreets.empty()) {
                std::cout << "Kamu tidak memiliki properti Street. Efek Festival tidak dapat digunakan.\n";
                break;
            }

            std::cout << "\nDaftar properti milikmu:\n";
            for (auto* s : ownedStreets) {
                std::cout << "- " << s->getShortName() << " (" << s->getName() << ")\n";
            }

            while (true) {
                std::cout << "Masukkan kode properti: ";
                std::string kode;
                std::getline(std::cin, kode);

                Street* chosen = nullptr;
                for (auto* s : ownedStreets) {
                    if (s->getShortName() == kode) { chosen = s; break; }
                }

                if (!chosen) {
                    std::cout << "-> Kode properti tidak valid atau bukan milikmu!\n";
                    continue;
                }

                int prevMult = chosen->getFestivalMultiplier();
                int prevSewa = chosen->getSewa();
                chosen->activateFestival();
                int newSewa = chosen->getSewa();
                int newMult = chosen->getFestivalMultiplier();

                if (prevMult == 1) {
                    std::cout << "\nEfek festival aktif!\n";
                    std::cout << "Sewa awal    : " << formatUangLocal(prevSewa) << "\n";
                    std::cout << "Sewa sekarang: " << formatUangLocal(newSewa) << "\n";
                    std::cout << "Durasi: 3 giliran\n";
                } else if (newMult > prevMult) {
                    std::cout << "\nEfek diperkuat!\n";
                    std::cout << "Sewa sebelumnya: " << formatUangLocal(prevSewa) << "\n";
                    std::cout << "Sewa sekarang  : " << formatUangLocal(newSewa) << "\n";
                    std::cout << "Durasi di-reset menjadi: 3 giliran\n";
                } else {
                    std::cout << "\nEfek sudah maksimum (harga sewa sudah digandakan tiga kali)\n";
                    std::cout << "Durasi di-reset menjadi: 3 giliran\n";
                }
                break;
            }
            break;
        }
    }
}
