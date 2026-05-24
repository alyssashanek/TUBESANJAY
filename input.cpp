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

// handle input keyboard biasa
void keyInput(unsigned char key, int x, int y) {
    key = tolower(key);
    keyStates[key] = true;
    switch (key) {
        // lihat belakang
        case 'c':
            lookBehind = true;
            break;

        // lihat kiri
        case 'q':
            lookLeft = true;
            break;
        
        // lihat kanan
        case 'e':
            lookRight = true;
            break;
        
        // ganti mode kamera first person view (fpv)
        case 'x':
            fpv = !fpv;
            break;
        
        // toggle mode headlight
        case 'h':
            headlightMode = (headlightMode + 1) % 4;
            
            // nyala dan matiin lampu
            if(headlightMode){
                glEnable(GL_LIGHT1);
                glEnable(GL_LIGHT2);
            } else {
                glDisable(GL_LIGHT1);
                glDisable(GL_LIGHT2);
            }
            angleY = (headlightMode == 1 ? -1.25 : -1);
            break;
        
        // reset posisi mobil dan lap
        case 'r':
            meX=240, meY=0, meZ=-40, angleX=0, angleY = (headlightMode == 2 ? -1 : -1.25);
            currentCheckpoint = 0;
            timerRunning = false;
            lapStartTime = 0;
            velocity = 0;
            currentLightRow = -1;
            updateLightSequence(0);
            break;
        
        // tombol esc
        case 27:
            std::cout << "ESC pressed. Right click to open menu.\n";
            break;
    }
    glutPostRedisplay();
}

// handle saat tombol keyboard dilepas
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

// handle tombol panah keyboard
void specialKeyInput(int key, int x, int y)
{
    switch(key){
        // maju tembus dinding
        case GLUT_KEY_UP:
            meZ=meZ+stepsize*cos(angleX*PI/180);
            meX=meX+stepsize*sin(angleX*PI/180);
            break;
        
        // mundur
        case GLUT_KEY_DOWN:
            meZ=meZ-stepsize*cos(angleX*PI/180);
            meX=meX-stepsize*sin(angleX*PI/180);
            break;
        
        // rotasi kanan
        case GLUT_KEY_RIGHT:
            angleX-=turnsize;
            break;

        // rotasi kiri
        case GLUT_KEY_LEFT:
            angleX+=turnsize;
            
            break;
    }
    glutPostRedisplay();
}

// menu klik kanan
void menu(int item) {
    switch(item) {
        // ganti siang/malam
        case 1:
            day = !day;

            if (day) {
                glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
            } else {
                glClearColor(0.05, 0.05, 0.15, 1.0);
            }
            break;

        // balik ke start menu
        case 2:
            gameStarted = false;
            timerRunning = false;
            lapStartTime = 0;
            velocity = 0;
            currentCheckpoint = 0;
            break;

        // keluar game
        case 3:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

// handle input mouse
void mouseInput(int button, int state, int x, int y) {
    // klik kiri pas di menu awal
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !gameStarted) {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        y = h - y;

        // ukuran tombol
        int buttonWidth = w * 0.25;
        int buttonHeight = 50;
        int startX = (w - buttonWidth) / 2;

        // posisi tombol
        int dayY = h * 0.60;
        int nightY = h * 0.50;
        int startY = h * 0.40;

        // cek klik di area tombol
        if (x >= startX && x <= startX + buttonWidth) {
            // tombol day
            if (y >= dayY && y <= dayY + buttonHeight) {
                isDaySelected = true;
                isNightSelected = false;
                day = true;
                glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
            }

            // tombol night
            else if (y >= nightY && y <= nightY + buttonHeight) {
                isDaySelected = false;
                isNightSelected = true;
                day = false;
                glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
            }

            // tombol start game
            else if (y >= startY && y <= startY + buttonHeight) {
                switchToMainGame();
            }
        }
        glutPostRedisplay();
    }
}

// bikin menu klik kanan
void createMenu() {
    int menuId = glutCreateMenu(menu);

    glutAddMenuEntry("Toggle Day/Night", 1);
    glutAddMenuEntry("Back to Start Menu", 2);
    glutAddMenuEntry("Exit Game", 3);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}