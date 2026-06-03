## Deskripsi
Furiosa Racing adalah game balap mobil 3D yang dibuat menggunakan bahasa pemrograman C++ dan library OpenGL. Proyek ini dikembangkan sebagai tugas mata kuliah Grafik dan Teknik Interaktif dengan tujuan menerapkan berbagai konsep yang telah dipelajari selama perkuliahan, seperti transformasi objek, sistem kamera, lighting, texture mapping, animasi, dan interaksi antar objek.

Pada game ini, pemain dapat mengendalikan mobil untuk berkeliling lintasan balap dan mencatat waktu tercepat dalam menyelesaikan satu putaran. Selain itu, tersedia pilihan mode siang dan malam yang memengaruhi pencahayaan lingkungan permainan.

## 🏁 Gameplay

1. Pilih mode **Day** atau **Night** pada menu awal.
2. Tekan tombol **Start** untuk memulai balapan.
3. Tunggu countdown lampu start selesai.
4. Selesaikan satu putaran lintasan dengan melewati seluruh checkpoint secara berurutan.
5. Sistem akan mencatat waktu lap dan menyimpan **Best Lap Time**.
6. Setelah menyelesaikan lap, pemain akan mendapatkan animasi kemenangan berupa confetti.

## Fitur
- Mobil balap yang dapat dikendalikan pemain
- Mode Day dan Night
- Kamera First Person dan Third Person
- Kamera melihat ke kiri, kanan, dan belakang
- Headlight pada mobil
- Speedometer
- Lap timer dan best lap timer
- Sistem checkpoint
- Efek kemenangan berupa confetti
- Lingkungan 3D yang terdiri dari jalan, rumput, pohon, bukit, awan, billboard, matahari, dan bulan

## Cara Menjalankan
Compile program menggunakan:
```bash
g++ main.cpp globals.cpp helpers.cpp environment.cpp track.cpp car.cpp ui.cpp input.cpp game.cpp -o game.exe -lfreeglut -lopengl32 -lglu32 -lwinmm
```
Jalankan dengan:
```bash
./game.exe
```
