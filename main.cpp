// g++ main.cpp globals.cpp helpers.cpp environment.cpp track.cpp car.cpp ui.cpp input.cpp game.cpp -o racingf.exe -lfreeglut -lopengl32 -lglu32 -lwinmm
// ./racingf.exe
#include <windows.h>
#include <mmsystem.h>
#include "globals.h"
#include "helpers.h"
#include "environment.h"
#include "track.h"
#include "car.h"
#include "ui.h"
#include "input.h"
#include "game.h"

void createMainWindow() {
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(0, 0);
    mainWindow = glutCreateWindow("FURIOSA RACING");

    setup();
    createMenu();
    loadGrassTexture();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyInput);
    glutMouseFunc(mouseInput);

    glutTimerFunc(0, update, 0);
    glutTimerFunc(0, globalTimer, 0);

    if (useIdleFunc) {
        glutIdleFunc(idle);
    }
}

// Main routine.
int main(int argc, char **argv) {
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    createMainWindow();
    PlaySound(TEXT("music/tokyodrift.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    glutMainLoop();
    return 0;
}