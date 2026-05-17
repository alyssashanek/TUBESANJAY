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
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, w, 0.0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);

    const char* title = "Select Mode";
    int titleWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)title);
    glRasterPos2i((w - titleWidth) / 2, h * 0.80);

    for (const char* c = title; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    int buttonWidth = w * 0.25;
    int buttonHeight = 50;
    int startX = (w - buttonWidth) / 2;

    int dayY = h * 0.60;
    int nightY = h * 0.50;
    int startY = h * 0.40;

    glColor3f(isDaySelected ? 0.0f : 0.3f, isDaySelected ? 1.0f : 0.6f, 0.0f);
    glRecti(startX, dayY, startX + buttonWidth, dayY + buttonHeight);
    drawButtonLabel("Day", startX, dayY + buttonHeight / 2, buttonWidth);

    glColor3f(isNightSelected ? 0.2f : 0.1f, isNightSelected ? 0.2f : 0.1f, isNightSelected ? 1.0f : 0.6f);
    glRecti(startX, nightY, startX + buttonWidth, nightY + buttonHeight);
    drawButtonLabel("Night", startX, nightY + buttonHeight / 2, buttonWidth);

    glColor3f(1.0f, 0.0f, 0.0f);
    glRecti(startX, startY, startX + buttonWidth, startY + buttonHeight);
    drawButtonLabel("Start Game", startX, startY + buttonHeight / 2, buttonWidth);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

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
