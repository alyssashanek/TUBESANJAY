#include "input.h"
#include "globals.h"
#include "game.h"

#include <cmath>
#include <cstdlib>
#include <cctype>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void keyInput(unsigned char key, int x, int y) {
    key = tolower(key);
    keyStates[key] = true;
    switch (key) {
        case 'c':
            lookBehind = true;
            break;
        case 'q':
            lookLeft = true;
            break;
        case 'e':
            lookRight = true;
            break;
        case 'x':
            fpv = !fpv;
            break;
        case 'h':
            headlightMode = (headlightMode + 1) % 4;  // Cycle through headlights
            if(headlightMode){
                glEnable(GL_LIGHT1);
                glEnable(GL_LIGHT2);
            } else {
                glDisable(GL_LIGHT1);
                glDisable(GL_LIGHT2);
            }
            angleY = (headlightMode == 1 ? -1.25 : -1);
            break;
        case 'r':
            meX=240, meY=0, meZ=-40, angleX=0, angleY = (headlightMode == 2 ? -1 : -1.25);
            currentCheckpoint = 0;
            timerRunning = false;
            lapStartTime = 0;
            velocity = 0;
            currentLightRow = -1;
            updateLightSequence(0);
            break;
        case 27:
            std::cout << "ESC pressed. Right click to open menu.\n";
            break;
    }
    glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y) {
    key = tolower(key);
    keyStates[key] = false;
    if (key == 'a' || key == 'd') {
        wheelAngle = 0.0f;
    }
    if (key == 'c') {
        lookBehind = false;
    } else if (key == 'q') {
        lookLeft = false;
    } else if (key == 'e') {
        lookRight = false;
    }
}
void specialKeyInput(int key, int x, int y)
{
    switch(key){
        // For clipping through walls
        case GLUT_KEY_UP:
            meZ=meZ+stepsize*cos(angleX*PI/180);
            meX=meX+stepsize*sin(angleX*PI/180);
            break;
        case GLUT_KEY_DOWN:
            meZ=meZ-stepsize*cos(angleX*PI/180);
            meX=meX-stepsize*sin(angleX*PI/180);
            break;
        case GLUT_KEY_RIGHT:
            angleX-=turnsize;
            break;
        case GLUT_KEY_LEFT:
            angleX+=turnsize;
            
            break;
    }
    glutPostRedisplay();
}

void menu(int item) {
    switch(item) {
        case 1:
            day = !day;

            if (day) {
                glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
            } else {
                glClearColor(0.05, 0.05, 0.15, 1.0);
            }
            break;

        case 2:
            gameStarted = false;
            timerRunning = false;
            lapStartTime = 0;
            velocity = 0;
            currentCheckpoint = 0;
            break;

        case 3:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

void mouseInput(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !gameStarted) {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        y = h - y;

        int buttonWidth = w * 0.25;
        int buttonHeight = 50;
        int startX = (w - buttonWidth) / 2;

        int dayY = h * 0.60;
        int nightY = h * 0.50;
        int startY = h * 0.40;

        if (x >= startX && x <= startX + buttonWidth) {
            if (y >= dayY && y <= dayY + buttonHeight) {
                isDaySelected = true;
                isNightSelected = false;
                day = true;
                glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
            }
            else if (y >= nightY && y <= nightY + buttonHeight) {
                isDaySelected = false;
                isNightSelected = true;
                day = false;
                glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
            }
            else if (y >= startY && y <= startY + buttonHeight) {
                switchToMainGame();
            }
        }

        glutPostRedisplay();
    }
}

void createMenu() {
    int menuId = glutCreateMenu(menu);

    glutAddMenuEntry("Toggle Day/Night", 1);
    glutAddMenuEntry("Back to Start Menu", 2);
    glutAddMenuEntry("Exit Game", 3);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}