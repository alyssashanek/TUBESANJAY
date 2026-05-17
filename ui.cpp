#include "ui.h"
#include "globals.h"
#include "environment.h"
#include "track.h"
#include "helpers.h"
#include "car.h"

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

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    if (isDaySelected) {
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        GLfloat globalAmbient[] = {0.75f, 0.75f, 0.75f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
        GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    }
    else {
        glClearColor(0.03f, 0.03f, 0.08f, 1.0f);
        GLfloat globalAmbient[] = {0.18f, 0.18f, 0.25f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
        GLfloat lightDiffuse[] = {0.35f, 0.35f, 0.45f, 1.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    }

    GLfloat sunlightPos[] = {250.0f, 350.0f, 250.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sunlightPos);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // proyeksi 3D untuk menggambar elemen latar belakang
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(70.0f, (float)w / (float)h, 1.0f, 3000.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // camera muter
    cameraAngle += 0.18f;
    float radius = 420.0f;
    float camX = sin(cameraAngle * PI / 180.0f) * radius;
    float camZ = cos(cameraAngle * PI / 180.0f) * radius;
    gluLookAt(camX, 180.0f, camZ, 0.0f, 0.0f, -40.0f, 0.0f, 1.0f, 0.0f);

    // gambar elemen latar belakang
    glEnable(GL_COLOR_MATERIAL);
    drawGrass();
    drawTrack();
    drawStartFinishLine();
    drawStartLight();
    GLfloat treeAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat treeDiffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};

    if (isDaySelected) {
        treeAmbient[0] =
        treeAmbient[1] =
        treeAmbient[2] = 0.5f;
        treeDiffuse[0] =
        treeDiffuse[1] =
        treeDiffuse[2] = 1.0f;
    }

    glLightfv(GL_LIGHT0, GL_AMBIENT, treeAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, treeDiffuse);

    drawTrees();
    drawClouds();
    drawBillboard();
    drawTrophy();

    if (isDaySelected)
        drawSun();
    else
        drawMoon();

    // player car di tengah layar
    glPushMatrix();
    glTranslatef(-35.0f, 2.0f, -5.0f);
    glRotatef(0.0f, 0, 1, 0);
    glScalef(1.15f, 1.15f, 1.15f);

    // simpan headlight lama
    int oldHeadlight = headlightMode;
    headlightMode = 0;

    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    drawRacecar();
    headlightMode = oldHeadlight;
    glPopMatrix();

    // proyeksi 2D untuk UI
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // proyeksi ortho untuk UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, w, 0.0, h);

    // modelview untuk UI
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // gambar overlay gelap
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);

    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(w, 0);
        glVertex2f(w, h);
        glVertex2f(0, h);
    glEnd();

    glDisable(GL_BLEND);

    // gambar judul
    glColor3f(1.0f, 1.0f, 1.0f);
    const char* title = "Select Mode";
    int titleWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)title);
    glRasterPos2i((w - titleWidth) / 2, h * 0.80);

    for (const char* c = title; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // gambar tombol
    int buttonWidth  = w * 0.25;
    int buttonHeight = 50;
    int startX = (w - buttonWidth) / 2;
    int dayY   = h * 0.60;
    int nightY = h * 0.50;
    int startY = h * 0.40;

    // DAY
    glColor3f(isDaySelected ? 0.0f : 0.3f, isDaySelected ? 1.0f : 0.6f, 0.0f);
    glRecti(startX, dayY, startX + buttonWidth, dayY + buttonHeight);
    drawButtonLabel("Day", startX, dayY + buttonHeight / 2, buttonWidth);

    // NIGHT
    glColor3f(isNightSelected ? 0.2f : 0.1f, isNightSelected ? 0.2f : 0.1f, isNightSelected ? 1.0f : 0.6f);
    glRecti(startX, nightY, startX + buttonWidth, nightY + buttonHeight);
    drawButtonLabel("Night", startX, nightY + buttonHeight / 2, buttonWidth);

    // START
    glColor3f(1.0f, 0.0f, 0.0f);
    glRecti(startX, startY, startX + buttonWidth, startY + buttonHeight);
    drawButtonLabel("Start Game", startX, startY + buttonHeight / 2, buttonWidth);

    // lap time & best lap
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // restore 3D projection dan modelview
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
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
