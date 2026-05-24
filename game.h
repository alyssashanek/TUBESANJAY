#ifndef GAME_H
#define GAME_H

// Inisialisasi OpenGL dan game
void setup();

// Mengatur ukuran window dan kamera
void resize(int w, int h);

// Mengatur checkpoint dan waktu lap
void updateCheckpoint(float x, float z);

// Mengecek collision area kotak
int isInsideAnyBox(float x, float z, float boxes[][6], int numBoxes);

// Mengecek apakah posisi mobil berada dalam rentang sudut tertentu
bool isWithinAngles(float x, float z, float cx, float cz, float startAngle, float endAngle);

// Mengecek collision area lingkaran
int isInsideAnyCircle(float x, float z, float circles[][7], int numCircles);

// Update game setiap frame
void update(int value);

// Render seluruh scene game
void drawScene();

// Update urutan lampu start
void updateLightSequence(int value);

// Timer global game
void globalTimer(int value);

// Update animasi saat idle
void idle();

// Memulai game utama
void switchToMainGame();

// Membuat window start screen
void createStartWindow();

// Membuat main game window
void createMainWindow();

// Menampilkan petunjuk kontrol game
void printInteraction();

#endif