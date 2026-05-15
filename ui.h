#ifndef UI_H
#define UI_H

void drawButtonLabel(
    const char* text,
    int x,
    int y,
    int width
);

void drawStartScreen();

void drawText(
    const char* string,
    int x,
    int y
);

void renderCenteredText(
    const char* string
);

void drawMPHDial(float mph);
void drawSpeedometerHUD();

#endif