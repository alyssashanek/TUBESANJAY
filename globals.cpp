#include "globals.h"

// Status game
bool gameStarted = false;

// Posisi mobil
float meX = 240;
float meY = 0;
float meZ = -40;

// Rotasi mobil dan arah lampu
float angleX = 0;
float angleY = -1;

// Kecepatan mobil
float velocity = 0.0;

// Percepatan mobil
float acceleration = 0.05;

// Perlambatan mobil
float deceleration = 0.02;

// Kecepatan maksimum mobil
float maxVelocity = 3.0;

// Status siang atau malam
bool day = true;

// Tekstur rumput dan billboard
GLuint textureGrass[1];
unsigned int textureBillboard[1];

// Kecepatan gerakan awan
float cloudSpeed = 0.05f;

// Confetti kemenangan
ConfettiParticle confettiCannon1[MAX_CONFETTI];
ConfettiParticle confettiCannon2[MAX_CONFETTI];

// Window untuk start screen dan main game
int startWindow;
int mainWindow;

// Mode update animasi saat idle
bool useIdleFunc = false;

// First person view
bool fpv = false;

// Kamera melihat ke belakang, kiri, atau kanan
bool lookBehind = false;
bool lookLeft = false;
bool lookRight = false;

// Status timer lap
bool timerRunning = false;

// Status pilihan siang atau malam di menu
bool isDaySelected = true;
bool isNightSelected = false;

// Status keyboard
bool keyStates[256] = {false};

// Checkpoint lap saat ini
int currentCheckpoint = 0;

// Lampu start yang aktif
int currentLightRow = -1;

// Waktu mulai dan selesai lap
int lapStartTime = 0;
int lapEndTime = 0;

// Waktu lap berjalan dan best lap
float currentLapTime = 0.0f;
float bestLapTime = 0.0f;

// Status apakah sudah ada best lap time
bool hasBestLapTime = false;

// Sudut roda depan
float wheelAngle = 0.0f;

// Rotasi piala
float trophyRotationAngle = 0.0f;

// Sudut kamera
float cameraAngle = 0.0f;

// Mode headlight
int headlightMode = 3;

// Sensitivitas belok mobil
float turnSpeed = 0.75f;

// Langkah gerakan dan rotasi tambahan
float stepsize = 1.0f;
float turnsize = 5.0f;

// Cahaya ambient global untuk seluruh scene 
float globAmb[4] = {0.2f,0.2f,0.2f,1.0f};

// Interval update lampu start (dalam milidetik)
int lightUpdateTime = 1000;