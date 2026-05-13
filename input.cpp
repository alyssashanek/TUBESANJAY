#include "input.h"
#include "globals.h"
#include "game.h"

#include <cmath>
#include <cstdlib>
#include <cctype>

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
        case 27: // ASCII code for the ESC key
            exit(0);
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
        case 1: // Toggle Day/Night
            day = !day; // Toggle the day variable
            
            if(day){glClearColor(0.53f, 0.81f, 0.92f, 1.0f);}
            else{glClearColor(0.05, 0.05, 0.15, 1.0);}
            break;
        case 2:
            break;
    }
    glutPostRedisplay(); // Redraw the scene to reflect the changes
}

void mouseInput(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Transform y coordinate
        y = windowHeight - y;  // Adjust for OpenGL's coordinate system (bottom-up)

        // Button dimensions and positions might also need to be scaled
        int startX = (int)(150 * (windowWidth / 500.0));  // Scale position based on window width
        int buttonWidth = (int)(200 * (windowWidth / 500.0));  // Scale width
        int buttonHeight = 50;  // Fixed height (can also scale if necessary)

        // Check button boundaries
        if (x > startX && x < startX + buttonWidth) {
            if (y > 600 * (windowHeight / 1000.0) && y < (600 * (windowHeight / 1000.0) + buttonHeight)) {
                isDaySelected = true;
                isNightSelected = false;
                day = true;
            } else if (y > 500 * (windowHeight / 1000.0) && y < (500 * (windowHeight / 1000.0) + buttonHeight)) {
                isDaySelected = false;
                isNightSelected = true;
                day = false;
            } else if (y > 400 * (windowHeight / 1000.0) && y < (400 * (windowHeight / 1000.0) + buttonHeight)) {
                switchToMainGame();
            }
            glutPostRedisplay();
        }
    }
}

void createMenu() {
    // Create a menu
    int menuId = glutCreateMenu(menu);

    // Add entries to the menu
    glutAddMenuEntry("Toggle Day/Night", 1);
    glutAddMenuEntry("Other Option", 2);

    // Attach the menu to the right mouse button
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}