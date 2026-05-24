#ifndef GLOBALS_H
#define GLOBALS_H

// Konstanta PI
#define PI 3.14159

// Jumlah maksimum confetti
#define MAX_CONFETTI 100

#include "structs.h"

#include <map>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Status game
extern bool gameStarted;

// Posisi mobil
extern float meX, meY, meZ;

// Rotasi mobil dan arah lampu
extern float angleX, angleY;

// Kecepatan mobil
extern float velocity;

// Percepatan mobil dan perlambatan mobil
extern float acceleration;
extern float deceleration;

// Kecepatan maksimum mobil
extern float maxVelocity;

// Status siang atau malam
extern bool day;

// Tekstur rumput dan billboard
extern GLuint textureGrass[1];
extern unsigned int textureBillboard[1];

// Posisi awan
extern CloudPosition cloudPositions[];;

// Kecepatan gerakan awan
extern float cloudSpeed;

// Confetti kemenangan
extern ConfettiParticle confettiCannon1[MAX_CONFETTI];
extern ConfettiParticle confettiCannon2[MAX_CONFETTI];

// Window untuk start screen dan main game
extern int startWindow;
extern int mainWindow;

// Mode update animasi saat idle
extern bool useIdleFunc;

// First person view
extern bool fpv;

// Kamera melihat ke belakang, kiri, atau kanan
extern bool lookBehind;
extern bool lookLeft;
extern bool lookRight;

// Status timer lap
extern bool timerRunning;

// Status pilihan siang atau malam di menu
extern bool isDaySelected;
extern bool isNightSelected;

// Status keyboard
extern bool keyStates[256];

// Checkpoint lap saat ini
extern int currentCheckpoint;

// Lampu start yang aktif
extern int currentLightRow;

// Waktu mulai dan selesai lap
extern int lapStartTime;
extern int lapEndTime;

// Waktu lap berjalan dan best lap
extern float currentLapTime;
extern float bestLapTime;

// Status apakah sudah ada best lap time
extern bool hasBestLapTime;

// Sudut roda depan
extern float wheelAngle;

// Rotasi piala
extern float trophyRotationAngle;

// Sudut kamera
extern float cameraAngle;

// Mode headlight
extern int headlightMode;

// Sensitivitas belok mobil
extern float turnSpeed;

// Langkah gerakan dan rotasi tambahan
extern float stepsize;
extern float turnsize;

// Cahaya ambient global untuk seluruh scene 
extern float globAmb[4];

// Interval update lampu start (dalam milidetik)
extern int lightUpdateTime;

#endif