#ifndef GLOBALS_H
#define GLOBALS_H
#define PI 3.14159
#define MAX_CONFETTI 100

#include "structs.h"
#include <map>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

extern bool gameStarted;

extern float meX, meY, meZ;
extern float angleX, angleY;

extern float velocity;
extern float acceleration;
extern float deceleration;
extern float maxVelocity;

extern bool day;

extern GLuint textureGrass[1];
extern unsigned int textureBillboard[1];

extern CloudPosition cloudPositions[];;

extern float cloudSpeed;

extern ConfettiParticle confettiCannon1[MAX_CONFETTI];
extern ConfettiParticle confettiCannon2[MAX_CONFETTI];

extern int startWindow;
extern int mainWindow;

extern bool useIdleFunc;

extern bool fpv;

extern bool lookBehind;
extern bool lookLeft;
extern bool lookRight;

extern bool timerRunning;

extern bool isDaySelected;
extern bool isNightSelected;

extern bool keyStates[256];

extern int currentCheckpoint;
extern int currentLightRow;

extern int lapStartTime;
extern int lapEndTime;

extern float currentLapTime;
extern float bestLapTime;
extern bool hasBestLapTime;

extern float wheelAngle;

extern float teapotRotationAngle;

extern float cameraAngle;

extern int headlightMode;

extern float turnSpeed;

extern float stepsize;
extern float turnsize;

extern float globAmb[4];

extern int lightUpdateTime;
#endif