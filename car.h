#ifndef CAR_H
#define CAR_H

// Mengatur posisi dan arah lampu mobil
void updateHeadlights();

// Membuat roda mobil
void drawWheel(float x, float y, float z, float angle);

// Membuat model mobil utama
void drawRacecar();

// Menampilkan HUD speedometer
void drawHUDGauge();

#endif