# Nimonspoli - CLI Monopoly Game in C++

## Deskripsi
Nimonspoli adalah permainan Monopoly yang diadaptasi untuk platform Command Line Interface (CLI). Proyek ini dikembangkan menggunakan bahasa pemrograman C++17 dengan menerapkan prinsip Object-Oriented Programming (OOP) seperti Tell, Don't Ask dan Runtime Polymorphism.

Program ini dilengkapi dengan fitur papan visual berukuran 11x11, pembacaan konfigurasi properti dari file eksternal (`data/property.txt`), dan manajemen status permainan.

---

## Struktur Direktori

Berikut adalah representasi struktur direktori proyek ini:

```text
.
|   .gitignore
|   makefile
|   README.md
|   
+---config/
+---data/
|       property.txt
|       
+---include/
|   +---core/         (Manajer permainan, pengontrol giliran, sistem pembayaran)
|   +---models/       (Entitas data seperti Player, Petak, Dice, Card)
|   +---utils/        (Utilitas sistem seperti ConfigParser, SaveManager)
|   \---views/        (Antarmuka CLI dan representasi visual papan)
|           
+---legacy/           (Implementasi terdahulu)
\---src/
    |   main.cpp
    +---core/         (Implementasi manajer inti)
    +---models/       (Implementasi entitas data)
    +---utils/        (Implementasi utilitas sistem)
    \---views/        (Implementasi antarmuka CLI)
```

---

## Persyaratan Sistem
- Compiler: Kompiler yang mendukung standar C++17 (contoh: GCC 7.3+, Clang 5.0+).
- Build Tool: GNU Make.
- Terminal: Terminal yang mendukung ANSI Escape Codes untuk pewarnaan dan format antarmuka.

---

## Cara Kompilasi

Proyek ini menggunakan Makefile untuk mengelola proses kompilasi modul-modul yang terbagi dalam layer core, models, views, dan utils.

1. Buka terminal.
2. Arahkan direktori aktif ke folder root proyek Nimonspoli.
3. Jalankan perintah berikut:

```bash
make
```

Catatan: Setelah kompilasi berhasil, file executable akan disimpan di `./bin/game` dan seluruh object files akan berada di `./build/`.

---

## Cara Menjalankan

Program sebaiknya dieksekusi dari root directory proyek agar file konfigurasi `data/property.txt` dapat ditemukan oleh sistem dengan benar.

Anda dapat menjalankan program melalui Makefile:
```bash
make run
```

Atau mengeksekusi file biner utamanya secara langsung:
```bash
./bin/game
```

---

## Daftar Perintah CLI

Setelah permainan berjalan dan data inisialisasi pemain dimasukkan, Anda dapat menggunakan perintah-perintah berikut pada giliran Anda:

| Perintah | Deskripsi |
| :--- | :--- |
| `LEMPAR_DADU` | Mengacak nilai dua buah dadu. Anda juga dapat menggunakan tombol Enter sebagai jalan pintas. |
| `ATUR_DADU X Y` | Mengatur nilai dadu secara manual menjadi X dan Y (dengan X, Y > 0). Perintah ini difasilitasi untuk kebutuhan pengujian. |
| `CETAK_PAPAN` | Menampilkan visual papan berukuran 11x11 beserta posisi bidak pemain dan status bangunan. |
| `CETAK_AKTA` | Menampilkan informasi rincian suatu properti, termasuk harga beli dan sewa. |
| `CETAK_PROPERTI <NAMA>` | Menampilkan daftar properti yang dikuasai oleh pemain yang dispesifikasikan. |
| `BANGUN` | Mendirikan bangunan (rumah atau hotel) pada properti yang memenuhi syarat. |
| `GADAI` | Menggadaikan properti kepada Bank untuk memperoleh dana instan. |
| `TEBUS` | Menebus kembali properti yang sebelumnya telah digadaikan. |
| `GUNAKAN_KEMAMPUAN` | Memakai kartu atau kemampuan khusus yang dimiliki oleh pemain. |
| `SIMPAN <file>` | Menyimpan sesi permainan saat ini ke dalam berkas tertentu. |
| `CETAK_LOG [n]` | Menampilkan riwayat transaksi dan aksi dalam permainan (menampilkan sebanyak `n` entri terakhir). |
| `HELP` | Menampilkan panduan dan daftar perintah yang dapat digunakan selama permainan berlangsung. |

---

## Mekanisme Permainan

- Pembayaran Sewa: Pemain yang mendarat di properti milik pemain lain akan membayar sewa secara otomatis kepada pemilik.
- Akuisisi Properti: Pemain diberikan pilihan untuk membeli properti bertipe Street. Untuk properti bertipe Railroad dan Utility yang belum berpemilik, hak milik akan diberikan secara otomatis tanpa pemotongan saldo saat pemain mendarat di petak tersebut.
- Penjara: Pemain akan langsung dipindahkan ke petak Jail apabila mendapatkan nilai dadu kembar (double) tiga kali berturut-turut pada giliran yang sama.
- Penanganan Input: Sistem dirancang untuk menangani berbagai format masukan pengguna demi mencegah terminasi program yang tidak semestinya akibat kesalahan format teks maupun spasi.

---

## Identitas Kelompok

**Kode Kelompok:** PSI  
**Nama Kelompok:** PengenSleep  

1. 13524113 / Fauzan Mohammad Abdul Ghani
2. 13524127 / Fazri Arrashyi Putra
3. 13524132 / Dika Pramudya Nugraha
4. 13524143 / Daniel Putra Rywandi S.
5. 13524145 / Dzakwan Muhammad Khairan Putra Purnama
