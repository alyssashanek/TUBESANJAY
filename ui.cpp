#include "ui.h"
#include "globals.h"

#include <cstring>
#include <cmath>
#include <cstdio>
#include <algorithm>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void drawButtonLabel(const char* text, int x, int y, int width) {
    glColor3f(1.0, 1.0, 1.0);  // Text color
    glRasterPos2i(x + (width - glutBitmapLength(GLUT_BITMAP_8_BY_13, (const unsigned char*)text)) / 2, y - 7);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }
}
void drawStartScreen() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 500, 0.0, 1000);  // Orthographic projection for 2D rendering

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);  // Set the background color

    glColor3f(1.0, 1.0, 1.0);  // Set text color to white
    glRasterPos2i(175, 800);  // Position the main title text
    const char* title = "Select Mode";
    for (const char* c = title; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    int startX = 150;
    int buttonWidth = 200;
    int buttonHeight = 50;

    // Day Button
    glColor3f(isDaySelected ? 0.0 : 0.3, isDaySelected ? 1.0 : 0.6, 0.0);  // Change color when selected
    glRecti(startX, 600, startX + buttonWidth, 650);
    drawButtonLabel("Day", startX, 625, buttonWidth);

    // Night Button
    glColor3f(isNightSelected * 0.2, isNightSelected * 0.2, isNightSelected ? 1.0 : 0.6);  // Change color when selected
    glRecti(startX, 500, startX + buttonWidth, 550);
    drawButtonLabel("Night", startX, 525, buttonWidth);

    // Play Game Button
    glColor3f(1.0, 0.0, 0.0);
    glRecti(startX, 400, startX + buttonWidth, 450);
    drawButtonLabel("Start Game", startX, 425, buttonWidth);

    glutSwapBuffers();
}

// Routine to draw a bitmap character string.
void drawText(const char* string, int x, int y) {
    glDisable(GL_LIGHTING);
    glColor3f(!day, !day, !day); // Set text color
    glRasterPos2i(x, y); // Position the text correctly
    while (*string) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *string++);
    }
    glEnable(GL_LIGHTING);
}
void renderCenteredText(const char* string) {
    glDisable(GL_LIGHTING);
    glColor3f(!day, !day, !day); // Set text color
    int x = glutGet(GLUT_WINDOW_WIDTH) / 2 - strlen(string) * 4.5; // Approximate center
    int y = glutGet(GLUT_WINDOW_HEIGHT) / 2 - 250;

    // Position the text in the middle of the screen
    glRasterPos2i(x, y);

    // Loop through each character in the string
    while(*string) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string++);
    }
    glEnable(GL_LIGHTING);
}

void drawMPHDial(float mph) {
    float gaugeHeight = 20.0f; // Height of the gauge
    int baseX = 10; // Base x position
    int baseY = glutGet(GLUT_WINDOW_HEIGHT) - 30;
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(baseX, baseY);
    glVertex2f(baseX + abs(mph), baseY);
    glVertex2f(baseX + abs(mph), baseY + 20);
    glVertex2f(baseX, baseY + 20);
    glEnd();
    drawText((velocity >= 0) ? "DRIVE" : "REVERSE", 10, 965);
}

