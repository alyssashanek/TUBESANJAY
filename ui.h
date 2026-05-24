#ifndef UI_H
#define UI_H

// gambar tulisan di tombol menu
void drawButtonLabel(
    const char* text,
    int x,
    int y,
    int width
);

// tampilan awal/menu sebelum game mulai
void drawStartScreen();

// render teks biasa di layar
void drawText(
    const char* string,
    int x,
    int y
);

// render teks di tengah layar
void renderCenteredText(
    const char* string
);

// indikator kecepatan model bar
void drawMPHDial(float mph);

// tampilan speedometer di layar game
void drawSpeedometerHUD();

#endif