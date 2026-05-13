#ifndef INPUT_H
#define INPUT_H

void keyInput(
    unsigned char key,
    int x,
    int y
);

void keyUp(
    unsigned char key,
    int x,
    int y
);

void specialKeyInput(
    int key,
    int x,
    int y
);

void menu(int item);

void createMenu();

void mouseInput(
    int button,
    int state,
    int x,
    int y
);

#endif