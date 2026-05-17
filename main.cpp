// cd /d/Programming/GTI/"Tugas Besar - Parts"
// g++ main.cpp car.cpp environment.cpp game.cpp globals.cpp helpers.cpp input.cpp track.cpp ui.cpp -o racingf -lfreeglut -lopengl32 -lglu32
// ./racingf

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
    mainWindow = glutCreateWindow("OpenGL Racing Simulator");

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

/*\ -------------------------- \*/
// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    createMainWindow();

    glutMainLoop();
    return 0;
}