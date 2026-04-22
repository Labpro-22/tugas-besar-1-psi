# Nimonspoli - CLI Monopoly Game in C++

## 🎲 Deskripsi
**Nimonspoli** adalah permainan papan legendaris Monopoly yang diadaptasi sepenuhnya ke dalam platform *Command Line Interface* (CLI). Dibangun menggunakan bahasa pemrograman **C++17** dengan fondasi *Object-Oriented Programming* (OOP) yang kokoh (mengadopsi prinsip *Tell, Don't Ask* dan *Runtime Polymorphism*). 

Permainan ini hadir dengan fitur papan visual (*hollow square* 11x11), sistem data dinamis yang membaca konfigurasi dari berkas eksternal (`property.txt`), manipulasi *string* `iomanip` yang rapi, serta *state-management* persisten selama sesi berlangsung.

---

## ⚙️ Persyaratan Sistem (Requirements)
- **Compiler:** Mendukung C++17 standar (misal: GCC 7.3+, Clang 5.0+).
- **Build Tool:** GNU Make.
- **Terminal:** Direkomendasikan menggunakan *Terminal Emulator* modern yang sepenuhnya mendukung sistem warna dan formating *ANSI Escape Codes* (WSL, Linux Terminal, macOS Terminal, atau Windows Terminal).

---

## 🛠️ Cara Build & Kompilasi (Compile)

Proyek ini menggunakan `Makefile` untuk manajemen modul kompilasi yang terotomatisasi (meliputi layer `core`, `models`, `views`, dan `utils`).

1. Buka *terminal* Anda.
2. Pastikan direktori aktif Anda (*Current Working Directory*) berada tepat di folder akar (*root*) proyek Nimonspoli.
3. Jalankan instruksi berikut untuk memicu kompilasi penuh:

```bash
make
```

> **Catatan:** Jika *build* sukses, file biner *executable* akan secara otomatis digenerasikan dan diletakkan pada path `./bin/game`. Semua *object files* akan diletakkan di dalam folder `./build/`.

---

## 🚀 Cara Menjalankan (Run)

Sangat penting untuk mengeksekusi program dari *root directory* dari proyek ini agar sistem *ConfigParser* berhasil melacak *path* konfigurasi file `data/property.txt` dengan benar.

Anda bisa menjalankannya via `Makefile`:
```bash
make run
```

Atau jika ingin mengeksekusi biner utamanya secara manual:
```bash
./bin/game
```

---

## 🕹️ Daftar Perintah CLI (Command List) [SEMENTARA]

Saat permainan dimulai, Anda akan diminta untuk memasukkan jumlah pemain dan nama setiap pemain. Selama giliran aktif (selama Anda belum menyentuh kondisi *End Turn*), Anda bebas mengeksekusi berbagai instruksi ini:

| Perintah | Deskripsi |
| :--- | :--- |
| `CETAK_PAPAN` | Mencetak visual 11x11 *board* Nimonspoli beserta letak bidak `(1)(2)...` dan tingkat bangunan rumah/hotel `(^)` yang presisi. |
| `LEMPAR_DADU` | Mengocok sepasang dadu (D6) secara acak. <br>*💡 Hint: Anda cukup menekan tombol `Enter` (kosong) sebagai shortcut pelemparan dadu.* |
| `ATUR_DADU X Y` | Memanipulasi angka sepasang dadu. Sangat berguna untuk kepentingan *Testing* dan proses penilaian asisten (contoh: `ATUR_DADU 1 4`). Pastikan `X` dan `Y` bernilai lebih dari 0. |
| `CETAK_AKTA` | Melihat informasi dan UI Mini-Akta lengkap milik sebuah properti (mencakup harga sewa bertingkat, kepemilikan, harga beli/gadai). Anda akan diminta memasukkan "KODE" singkat properti (contoh: `GBR`, `JKT`, `PLN`). |
| `HELP` | Menampilkan panduan bantuan ringkas tentang seluruh perintah di atas selama *runtime*. |

---

## 🌟 Fitur Inti Mekanik

- **Auto-Rent Enforcement:** Berkat implementasi prinsip "*Tell, Don't Ask*", pemain yang mendarat di properti musuh akan langsung ditarik saldo sewanya secara sistematis.
- **Auto-Acquisition:** Membeli petak jalan biasa (`Street`) akan memunculkan *prompt* `(Y/N)`, tetapi mendarat di Stasiun (`Railroad`) atau Perusahaan (`Utility`) kosong akan langsung diberikan secara gratis (Otomatis).
- **Rule of Threes (Penjara):** Mendapatkan lemparan dadu kembar (Double) 3 kali berturut-turut akan dianggap ngebut dan pemain dijebloskan langsung ke JAIL.
- **Anti-Crash Parsing:** Validasi input *commands* telah dirancang sangat *robust*. Kesalahan pengetikan, kelebihan spasi, atau input di luar batasan tidak akan menyebabkan program *Segmentation Fault* berkat manajemen proteksi tipe dinamis di C++17.
