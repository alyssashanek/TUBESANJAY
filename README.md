## Deskripsi
Furiosa Racing adalah game balap mobil 3D yang dibuat menggunakan bahasa pemrograman C++ dan library OpenGL. Proyek ini dikembangkan sebagai tugas mata kuliah Grafika Komputer dengan tujuan menerapkan berbagai konsep yang telah dipelajari selama perkuliahan, seperti transformasi objek, sistem kamera, lighting, texture mapping, animasi, dan interaksi antar objek.

Pada game ini, pemain dapat mengendalikan mobil untuk berkeliling lintasan balap dan mencatat waktu tercepat dalam menyelesaikan satu putaran. Selain itu, tersedia pilihan mode siang dan malam yang memengaruhi pencahayaan lingkungan permainan.

## Fitur
- Mobil balap yang dapat dikendalikan pemain
- Mode Day dan Night
- Kamera First Person dan Third Person
- Kamera melihat ke kiri, kanan, dan belakang
- Headlight pada mobil
- Speedometer
- Lap timer dan best lap timer
- Sistem checkpoint
- Efek kemenangan berupa trofi dan confetti
- Lingkungan 3D yang terdiri dari jalan, rumput, pohon, bukit, awan, billboard, matahari, dan bulan

## Cara Menjalankan
Compile program menggunakan:
```bash
g++ main.cpp globals.cpp helpers.cpp environment.cpp track.cpp car.cpp ui.cpp input.cpp game.cpp -o racingf.exe -lfreeglut -lopengl32 -lglu32 -lwinmm
