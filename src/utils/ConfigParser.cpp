#include "utils/ConfigParser.hpp"
#include "models/Street.hpp"
#include "models/Railroad.hpp"
#include "models/Utility.hpp"
#include "models/PetakAksi.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::unique_ptr<Petak>> ConfigParser::loadBoardConfig(const std::string& filename) {
    // 1. Inisialisasi vector dengan ukuran pasti 40
    std::vector<std::unique_ptr<Petak>> board(40);

    auto setAksi = [&](int index, const std::string& nama, const std::string& kode, ActionType type, int amount = 0) {
        board[index] = std::make_unique<PetakAksi>(index, nama, type, amount);
        board[index]->setSkin(kode, "DF");
    };

    // 2. Isi terlebih dahulu petak-petak aksi (Action Tiles)
    setAksi(0, "GO", "GO", ActionType::GO, 200);
    setAksi(2, "Dana Umum", "DNU", ActionType::FREE_PARKING);
    setAksi(4, "Pajak Penghasilan", "PPH", ActionType::TAX, 200);
    setAksi(7, "Festival", "FES", ActionType::FREE_PARKING);
    setAksi(10, "Penjara", "PEN", ActionType::FREE_PARKING);
    setAksi(17, "Dana Umum", "DNU", ActionType::FREE_PARKING);
    setAksi(20, "Bebas Parkir", "BBP", ActionType::FREE_PARKING);
    setAksi(22, "Kesempatan", "KSP", ActionType::FREE_PARKING);
    setAksi(30, "Petak Pergi ke Penjara", "PPJ", ActionType::JAIL);
    setAksi(33, "Festival", "FES", ActionType::FREE_PARKING);
    setAksi(36, "Kesempatan", "KSP", ActionType::FREE_PARKING);
    setAksi(38, "Pajak Barang Mewah", "PBM", ActionType::TAX, 200);

    // 3. Buka file property.txt
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filename << ". Board is incomplete!\n";
        // 7. Jika error, tetap pastikan tidak ada nullptr sebelum return
        for (int i = 0; i < 40; i++) {
            if (!board[i]) {
                setAksi(i, "Unknown", "???", ActionType::FREE_PARKING);
            }
        }
        return board;
    }

    std::string line;
    // Abaikan baris pertama (header)
    std::getline(file, line);

    // 4. Baca baris per baris
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '\r' || line[0] == '#') continue;

        std::stringstream ss(line);
        int id;
        std::string kode, nama, jenis, warna;
        
        if (!(ss >> id >> kode >> nama >> jenis >> warna)) continue;

        if (id < 1 || id > 40) continue;
        
        // 5. Masukkan objek hasil parsing ke array indeks (ID - 1)
        int index = id - 1; 

        // 6. Logika Parsing berdasarkan JENIS
        if (jenis == "STREET") {
            int hargaLahan, nilaiGadai, upgRumah, upgHotel;
            ss >> hargaLahan >> nilaiGadai >> upgRumah >> upgHotel;

            std::vector<int> rents;
            std::string rentToken;
            while (ss >> rentToken) {
                if (rentToken == "…") continue; // Abaikan karakter elipsis dari format copas
                try {
                    rents.push_back(std::stoi(rentToken));
                } catch(...) {}
            }
            while (rents.size() < 6) rents.push_back(0); // Pad ke 6 level jika data mentah kurang

            std::string colorCode = "DF";
            if (warna == "COKLAT") colorCode = "CK";
            else if (warna == "BIRU_MUDA") colorCode = "BM";
            else if (warna == "MERAH_MUDA") colorCode = "PK";
            else if (warna == "ORANGE") colorCode = "OR";
            else if (warna == "MERAH") colorCode = "MR";
            else if (warna == "KUNING") colorCode = "KN";
            else if (warna == "HIJAU") colorCode = "HJ";
            else if (warna == "BIRU_TUA") colorCode = "BT";

            board[index] = std::make_unique<Street>(index, nama, warna, hargaLahan, rents, upgRumah, upgHotel);
            board[index]->setSkin(kode, colorCode);

        } else if (jenis == "RAILROAD") {
            int hargaLahan, nilaiGadai;
            ss >> hargaLahan >> nilaiGadai;
            board[index] = std::make_unique<Railroad>(index, nama, hargaLahan, 25);
            board[index]->setSkin(kode, "DF");

        } else if (jenis == "UTILITY") {
            int hargaLahan, nilaiGadai;
            ss >> hargaLahan >> nilaiGadai;
            board[index] = std::make_unique<Utility>(index, nama, hargaLahan);
            board[index]->setSkin(kode, "AB");
        }
    }

    // Pastikan tidak ada nullptr pada petak yang mungkin terlewat di text file
    for (int i = 0; i < 40; i++) {
        if (!board[i]) {
            setAksi(i, "Unknown", "???", ActionType::FREE_PARKING);
        }
    }

    return board;
}
