#ifndef INPUT_H
#define INPUT_H

// input keyboard biasa
void keyInput(unsigned char key, int x, int y);

// kondisi saat tombol dilepas
void keyUp(unsigned char key, int x, int y);

// handle tombol panah keyboard
void specialKeyInput(int key, int x, int y);

// fungsi menu klik kanan
void menu(int item);

// bikin menu klik kanan
void createMenu();

// handle input mouse
void mouseInput(int button, int state, int x, int y);

#endif