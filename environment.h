#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "structs.h"

// Memuat tekstur rumput
void loadGrassTexture();

// Membuat permukaan rumput
void drawGrass();

// Memuat tekstur billboard
void loadBillboardTexture();

// Membuat billboard lintasan
void drawBillboard();

// Membuat satu awan
void drawCloud(float x, float y, float z);

// Menampilkan semua awan
void drawClouds();

// Membuat bukit
void drawHill(float x, float y, float z, float scale);

// Membuat satu pohon
void drawTree(float x, float y, float z, float trunkHeight, float foliageRadius);

// Menampilkan semua pohon
void drawTrees();

// Membuat matahari
void drawSun();

// Membuat bulan
void drawMoon();

// Inisialisasi partikel confetti
void initConfetti(ConfettiParticle confetti[], float posX, float posY, float posZ);

// Update dan menampilkan confetti
void updateAndDrawConfetti(ConfettiParticle confetti[]);

// Membuat piala
void drawTrophy(void);

#endif