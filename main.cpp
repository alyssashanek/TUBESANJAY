#include "globals.h"
#include "helpers.h"
#include "environment.h"
#include "track.h"
#include "car.h"
#include "ui.h"
#include "input.h"
#include "game.h"

void createStartWindow() {
    glutInitWindowSize(500, 1000);
    glutInitWindowPosition(1000, 0);
    startWindow = glutCreateWindow("Start Screen");

    glutDisplayFunc(drawStartScreen);
    glutReshapeFunc(startScreenResize);
    glutMouseFunc(mouseInput);
}
void createMainWindow() {
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(0, 0);
    mainWindow = glutCreateWindow("OpenGL Racing Simulator");

    setup();  // Setup your OpenGL context and initial states for the main game
    createMenu();
    loadGrassTexture();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyInput);
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
    
    createStartWindow();
    createMainWindow();

    glutMainLoop();
    return 0;
}