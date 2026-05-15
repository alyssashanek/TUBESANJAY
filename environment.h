#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "structs.h"

void loadGrassTexture();
void drawGrass();

void loadBillboardTexture();
void drawBillboard();

void drawCloud(float x, float y, float z);
void drawClouds();

void drawHill(float x, float y, float z, float scale);

void drawTree(
    float x,
    float y,
    float z,
    float trunkHeight,
    float foliageRadius
);

void drawTrees();

void drawSun();
void drawMoon();

void initConfetti(
    ConfettiParticle confetti[],
    float posX,
    float posY,
    float posZ
);

void updateAndDrawConfetti(
    ConfettiParticle confetti[]
);

void drawTeapot();

#endif