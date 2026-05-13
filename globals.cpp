#include "globals.h"

bool gameStarted = false;

float meX = 240;
float meY = 0;
float meZ = -40;

float angleX = 0;
float angleY = -1;

float velocity = 0.0;
float acceleration = 0.05;
float deceleration = 0.02;
float maxVelocity = 3.0;

bool day = true;

GLuint textureGrass[1];

float cloudSpeed = 0.1f;

ConfettiParticle confettiCannon1[MAX_CONFETTI];
ConfettiParticle confettiCannon2[MAX_CONFETTI];

int startWindow;
int mainWindow;

bool useIdleFunc = false;

bool fpv = false;

bool lookBehind = false;
bool lookLeft = false;
bool lookRight = false;

bool timerRunning = false;

bool isDaySelected = true;
bool isNightSelected = false;

bool keyStates[256] = {false};

int currentCheckpoint = 0;
int currentLightRow = -1;

int lapStartTime = 0;
int lapEndTime = 0;

float currentLapTime = 0.0f;

float wheelAngle = 0.0f;

float teapotRotationAngle = 0.0f;

float cameraAngle = 0.0f;

int headlightMode = 3;

float turnSpeed = 0.5f;

float stepsize = 1.0f;
float turnsize = 5.0f;

float globAmb[4] = {0.2f,0.2f,0.2f,1.0f};

int lightUpdateTime = 1000;