#ifndef GAME_H
#define GAME_H

void setup();

void resize(int w, int h);

void updateCheckpoint(
    float x,
    float z
);

int isInsideAnyBox(
    float x,
    float z,
    float boxes[][6],
    int numBoxes
);

bool isWithinAngles(
    float x,
    float z,
    float cx,
    float cz,
    float startAngle,
    float endAngle
);

int isInsideAnyCircle(
    float x,
    float z,
    float circles[][7],
    int numCircles
);

void update(int value);

void drawScene();

void updateLightSequence(int value);

void globalTimer(int value);

void idle();

void startScreenResize(int w, int h);

void switchToMainGame();

void createStartWindow();

void createMainWindow();

void printInteraction();

#endif