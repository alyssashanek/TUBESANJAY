#include "car.h"
#include "globals.h"
#include "helpers.h"
#include "ui.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Quadric object untuk mobil
static GLUquadric* carQ = nullptr;

// Membuat quadric jika belum ada
static GLUquadric* getCarQ() {
    if (carQ == nullptr) {
        carQ = gluNewQuadric();
    }
    return carQ;
}

// Helper untuk membuat cube dengan posisi dan ukuran tertentu
static void lb(float tx, float ty, float tz,
               float sx, float sy, float sz) {
    glPushMatrix();
    glTranslatef(tx, ty, tz);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Warna body dan komponen mobil
static void cBlue()   {glColor3f(0.25f, 0.55f, 0.85f);}
static void cDkBlue() {glColor3f(0.15f, 0.38f, 0.65f);}
static void cLtBlue() {glColor3f(0.45f, 0.72f, 0.95f);}
static void cBlack()  {glColor3f(0.08f, 0.08f, 0.10f);}
static void cDkGray() {glColor3f(0.22f, 0.22f, 0.24f);}
static void cGray()   {glColor3f(0.42f, 0.42f, 0.45f);}
static void cLtGray() {glColor3f(0.72f, 0.72f, 0.75f);}
static void cSilver() {glColor3f(0.82f, 0.83f, 0.86f);}
static void cOrange() {glColor3f(0.85f, 0.40f, 0.10f);}
static void cAmber()  {glColor3f(0.95f, 0.78f, 0.20f);}

// Membuat body utama mobil
static void drawMainBody() {

    // base body
    cDkBlue();
    lb(0, -0.18f, 0, 3.6f, 0.20f, 7.6f);
    cBlue();   
    lb(0, 0.26f, -0.1f, 3.5f, 0.72f, 7.4f);

    // Fender depan & belakang
    cBlue();
    lb(-1.92f, 0.22f, -2.2f, 0.28f, 0.55f, 1.8f);
    lb( 1.92f, 0.22f, -2.2f, 0.28f, 0.55f, 1.8f);
    lb(-1.92f, 0.22f, 2.1f, 0.28f, 0.60f, 2.0f);
    lb( 1.92f, 0.22f, 2.1f, 0.28f, 0.60f, 2.0f);

    // Kap mobil
    cLtBlue();
    glPushMatrix(); 
    glTranslatef(0, 0.63f, -2.2f); 
    glScalef(3.4f, 0.12f, 3.0f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    cBlue();
    glPushMatrix(); 
    glTranslatef(0, 0.68f, -0.9f); 
    glScalef(3.4f, 0.10f, 1.0f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    // Atap mobil
    cLtBlue(); 
    lb(0, 1.42f, 0.5f, 3.2f, 0.14f, 3.5f);
    glColor3f(0.50f, 0.75f, 0.95f);
    glPushMatrix(); 
    glTranslatef(0, 1.36f, 0.5f); 
    glScalef(3.1f, 0.30f, 3.3f); 
    glutSolidSphere(0.52f, 16, 10); 
    glPopMatrix();

    // Bagian belakang mobil
    cBlue();
    glPushMatrix(); 
    glTranslatef(0, 1.05f, 2.85f); 
    glRotatef(-30, 1, 0, 0); 
    glScalef(3.2f, 0.12f, 1.4f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    cDkBlue(); 
    lb(0, 0.70f, 3.5f, 3.2f, 0.70f, 0.18f);

    // Bumper
    cDkGray(); 
    lb(0, -0.05f, -3.9f,  3.5f, 0.30f, 0.20f);
    cBlack();  
    lb(0, -0.25f, -3.85f, 3.3f, 0.08f, 0.30f);
    cDkGray(); 
    lb(0, 0.05f, 3.75f, 3.4f, 0.55f, 0.22f);
}

// Membuat grille depan mobil
static void drawFrontGrille() {

    // Base grille
    cBlack(); 
    lb(0, 0.10f, -3.92f, 2.6f, 0.50f, 0.10f);

    // Garis grille utama
    cDkGray();
    float barY[5] = {-0.10f, 0.02f, 0.14f, 0.26f, 0.38f};
    
    for (int i = 0; i < 5; i++) {
        lb(0, 0.10f + barY[i], -3.90f, 2.5f, 0.055f, 0.12f);
    }

    // Detail tengah
    cGray(); 
    lb(0, 0.18f, -3.93f, 0.38f, 0.22f, 0.08f);

    // Upper grille
    cBlack(); 
    lb(0, 0.52f, -3.80f, 2.0f, 0.22f, 0.12f);
    cDkGray();
    float ub[3] = {0.0f, 0.09f, 0.18f};

    for (int i = 0; i < 3; i++) {
        lb(0, 0.52f + ub[i], -3.79f, 1.9f, 0.04f, 0.10f);
    }
}

// Ventilasi pada kap mobil
static void drawHoodVents() {

    // Ventilasi kiri
    cBlack(); 
    lb(-0.65f, 0.70f, -2.0f, 0.70f, 0.10f, 1.0f);
    cDkGray();

    for (int i = 0; i < 4; i++) {
        lb(-0.65f, 0.71f, -2.35f + i * 0.22f, 0.65f, 0.06f, 0.06f);
    }

    // Ventilasi kanan
    cBlack(); 
    lb(0.65f, 0.70f, -2.0f, 0.70f, 0.10f, 1.0f);
    cDkGray();

    for (int i = 0; i < 4; i++) {
        lb(0.65f, 0.71f, -2.35f + i * 0.22f, 0.65f, 0.06f, 0.06f);
    }
}

// Membuat lampu depan mobil
static void drawHeadlightsBody() {

    // Lampu kiri
    cDkGray(); 
    lb(-1.15f, 0.42f, -3.82f, 0.90f, 0.38f, 0.14f);
    glColor3f(0.80f, 0.88f, 0.95f); 
    lb(-1.15f, 0.42f, -3.84f, 0.82f, 0.30f, 0.08f);
    cOrange();  
    lb(-1.15f, 0.26f, -3.84f, 0.82f, 0.08f, 0.08f);
    cAmber();   
    lb(-1.10f, 0.42f, -3.85f, 0.30f, 0.18f, 0.06f);

    // Lampu kanan
    cDkGray(); 
    lb(1.15f, 0.42f, -3.82f, 0.90f, 0.38f, 0.14f);
    glColor3f(0.80f, 0.88f, 0.95f); 
    lb(1.15f, 0.42f, -3.84f, 0.82f, 0.30f, 0.08f);
    cOrange();  
    lb(1.15f, 0.26f, -3.84f, 0.82f, 0.08f, 0.08f);
    cAmber();   
    lb(1.10f, 0.42f, -3.85f, 0.30f, 0.18f, 0.06f);
}

// Membuat lampu belakang mobil
static void drawTaillights() {

    // Lampu kiri
    cDkGray(); 
    lb(-1.10f, 0.80f, 3.76f, 0.90f, 0.40f, 0.14f);
    glColor3f(0.85f, 0.15f, 0.10f); 
    lb(-1.10f, 0.80f, 3.78f, 0.82f, 0.32f, 0.08f);
    cAmber(); 
    lb(-1.10f, 0.65f, 3.78f, 0.82f, 0.08f, 0.08f);

    // Lampu kanan
    cDkGray(); 
    lb(1.10f, 0.80f, 3.76f, 0.90f, 0.40f, 0.14f);
    glColor3f(0.85f, 0.15f, 0.10f); 
    lb(1.10f, 0.80f, 3.78f, 0.82f, 0.32f, 0.08f);
    cAmber(); lb(1.10f, 0.65f, 3.78f, 0.82f, 0.08f, 0.08f);
}

// Membuat kaca mobil
static void drawWindows() {

    // Transparansi kaca
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Kaca depan
    glColor4f(0.05f, 0.10f, 0.20f, 0.85f);
    glPushMatrix(); 
    glTranslatef(0, 1.08f, -1.28f); 
    glRotatef(55, 1, 0, 0);
    glScalef(3.0f, 0.10f, 1.60f); 
    glutSolidCube(1.0f);
    glPopMatrix();

    // Kaca belakang
    glColor4f(0.05f, 0.10f, 0.20f, 0.85f);
    glPushMatrix(); 
    glTranslatef(0, 1.05f, 2.40f); 
    glRotatef(-38, 1, 0, 0);
    glScalef(3.0f, 0.10f, 1.30f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    // Kaca samping
    glColor4f(0.10f, 0.18f, 0.30f, 0.75f);
    glPushMatrix();
    glTranslatef(-1.80f, 1.05f, -0.50f); 
    glScalef(0.08f, 0.60f, 1.80f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    glPushMatrix(); 
    glTranslatef(-1.80f, 1.05f, 1.40f); 
    glScalef(0.08f, 0.55f, 1.40f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    glPushMatrix(); 
    glTranslatef(1.80f, 1.05f, -0.50f); 
    glScalef(0.08f, 0.60f, 1.80f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    glPushMatrix(); 
    glTranslatef(1.80f, 1.05f, 1.40f); 
    glScalef(0.08f, 0.55f, 1.40f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    glDisable(GL_BLEND);

    // Frame kaca 
    cDkGray();
    glPushMatrix(); 
    glTranslatef(-1.62f, 0.98f, -1.18f);  
    glRotatef(-52, 1, 0, 0);
    glScalef(0.10f, 0.10f, 1.35f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    glPushMatrix(); 
    glTranslatef(1.62f, 0.98f, -1.18f); 
    glRotatef(-52, 1, 0, 0);
    glScalef(0.10f, 0.10f, 1.35f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    // Frame atas kaca samping
    lb(-1.60f, 1.42f, 0.5f, 0.10f, 0.10f, 3.5f);
    lb( 1.60f, 1.42f, 0.5f, 0.10f, 0.10f, 3.5f);
}

// Membuat spoiler belakang mobil
static void drawRearSpoiler() {

    // Penyangga spoiler
    cDkGray();
    lb(-1.0f, 1.55f, 2.85f, 0.12f, 0.28f, 0.12f);
    lb(1.0f, 1.55f, 2.85f, 0.12f, 0.28f, 0.12f);

    // Bagian utama spoiler
    cDkBlue();
    glPushMatrix(); 
    glTranslatef(0, 1.70f, 2.85f); 
    glRotatef(-8, 1, 0, 0);
    glScalef(3.0f, 0.14f, 0.55f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    // Detail spoiler
    cBlack();
    glPushMatrix(); 
    glTranslatef(0, 1.78f, 3.05f); 
    glScalef(3.0f, 0.20f, 0.06f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    // Sisi spoiler
    cDkGray();
    lb(-1.52f, 1.70f, 2.88f, 0.10f, 0.22f, 0.60f);
    lb(1.52f, 1.70f, 2.88f, 0.10f, 0.22f, 0.60f);
}

// Membuat spion mobil
static void drawMirrors() {

    // Body spion
    cBlue();
    lb(-1.82f, 1.12f, -1.55f, 0.35f, 0.12f, 0.22f);
    lb(1.82f, 1.12f, -1.55f, 0.35f, 0.12f, 0.22f);

    // Penyangga spion
    cBlack();
    lb(-1.92f, 1.12f, -1.55f, 0.08f, 0.14f, 0.22f);
    lb(1.92f, 1.12f, -1.55f, 0.08f, 0.14f, 0.22f);
}

// Membuat garis dan handle pintu mobil
static void drawDoorLines() {

    // Garis pintu
    cDkBlue();
    lb(-1.78f, 0.32f, 0.4f, 0.04f, 0.04f, 6.8f);
    lb(1.78f, 0.32f, 0.4f, 0.04f, 0.04f, 6.8f);

    // Handle pintu
    cLtGray();
    lb(-1.83f, 0.60f, -0.35f, 0.08f, 0.08f, 0.35f);
    lb(1.83f, 0.60f, -0.35f, 0.08f, 0.08f, 0.35f);
    lb(-1.83f, 0.60f,  1.20f, 0.08f, 0.08f, 0.35f);
    lb(1.83f, 0.60f,  1.20f, 0.08f, 0.08f, 0.35f);
}

// Membuat bagian bawah mobil
static void drawUnderbody() {

    // Dasar mobil
    cBlack(); 
    lb(0, -0.30f, 0, 3.2f, 0.08f, 7.0f);

    // Knalpot
    cDkGray();
    glPushMatrix();
    glTranslatef(-0.6f, -0.28f, 3.70f);
    glRotatef(90, 1, 0, 0);
    gluCylinder(getCarQ(), 0.09f, 0.09f, 0.30f, 12, 1);
    glPopMatrix();

    // Detail bawah depan
    lb(0, -0.30f, -3.3f, 2.8f, 0.10f, 0.80f);
}

// Membuat roda mobil
void drawWheel(float tx, float ty, float tz, float angle) {
    glPushMatrix();
    glTranslatef(tx, ty, tz);

    // Sudut belok roda
    glRotatef(angle, 0, 1, 0);

    // Orientasi dasar roda (roda depan lebih miring)
    glRotatef(90.0f, 0, 1, 0);

    // Ban utama
    glColor3f(0.09f, 0.09f, 0.09f);
    glutSolidTorus(0.30f, 0.62f, 20, 48);

    // Sisi ban
    glColor3f(0.16f, 0.16f, 0.16f);
    glPushMatrix(); 
    glScalef(1.0f, 0.06f, 1.0f);
    glutSolidTorus(0.25f, 0.62f, 12, 36); 
    glPopMatrix();

    // Pelek luar
    cSilver(); 
    glutSolidTorus(0.13f, 0.34f, 12, 32);

    // Jari-jari pelek
    cLtGray();
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glRotatef(i * 72.0f, 0, 0, 1);
        glTranslatef(0, 0.22f, 0);
        glScalef(0.07f, 0.30f, 0.06f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Tengah pelek
    cGray();   
    glutSolidTorus(0.06f, 0.14f, 10, 20);
    cSilver(); 
    glutSolidSphere(0.10f, 16, 12);

    // Kaliper rem
    cOrange();
    glPushMatrix(); 
    glTranslatef(0.08f, 0.32f, 0);
    glScalef(0.12f, 0.18f, 0.25f); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    glPopMatrix();
}

// Mengatur posisi dan arah lampu depan berdasarkan sudut mobil
void updateHeadlights() {

    // Konversi sudut mobil ke arah lampu (radian)
    float rad = angleX * PI / 180.0;

    // Arah lampu berdasarkan sudut mobil
    float lightDirX = sin(rad);
    float lightDirZ = cos(rad);

    // Posisi lampu kiri
    GLfloat light0Pos[] = {
        meX - lightDirZ * 2 + lightDirX * 18.0f, 5.0f,
        meZ + lightDirX * 2 + lightDirZ * 18.0f, 1.0f
    };

    // Posisi lampu kanan
    GLfloat light1Pos[] = {
        meX + lightDirZ * 2 + lightDirX * 18.0f, 5.0f,
        meZ - lightDirX * 2 + lightDirZ * 18.0f, 1.0f
    };

    // Arah sorot lampu
    GLfloat lightDir[] = {
        lightDirX, angleY, lightDirZ
    };

    // Update lampu kiri
    glLightfv(GL_LIGHT1, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir);

    // Update lampu kanan
    glLightfv(GL_LIGHT2, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDir);
}

// Menampilkan HUD speedometer saat dalam mode FPV
void drawHUDGauge() {

    // HUD hanya muncul saat mode FPV aktif
    if (fpv != 1) {
        return;
    }

    // Setup proyeksi 2D untuk HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1366, 0, 768);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Nonaktifkan lighting dan depth
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Atifkan transparansi
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float cx = 683.0f;
    float cy = 70.0f;

    // Gauge kiri
    glColor4f(0,0,0,0.35f);
    drawCircleXY(cx - 185, cy - 10, 0, 85);
    glColor4f(1,1,1,0.22f);
    drawCircleXY(cx - 185, cy - 10, 0, 90);

    // Gauge kanan
    glColor4f(0,0,0,0.35f);
    drawCircleXY(cx + 185, cy - 10, 0, 85);
    glColor4f(1,1,1,0.22f);
    drawCircleXY(cx + 185, cy - 10, 0, 90);

    // Gauge utama
    glColor4f(0,0,0,0.45f);
    drawCircleXY(cx, cy, 0, 140);
    glColor4f(0.03f, 0.08f, 0.20f, 0.9f);
    drawCircleXY(cx, cy, 0, 146);

    glDisable(GL_BLEND);

    // Hitung kecepatan
    int mph = abs((int)(velocity * 40));
    char txt[16];
    sprintf(txt, "%03d", mph);
    glColor3f(1,1,1);

    // Text speed
    glPushMatrix();
    glTranslatef(cx - 38, cy - 20, 0);
    glScalef(0.32f, 0.32f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, txt);
    glPopMatrix();

    // Label "SPEED"
    glPushMatrix();
    glTranslatef(cx - 32, cy + 55, 0);
    glScalef(0.16f, 0.16f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, "SPEED");
    glPopMatrix();

    // Gear mobil (R untuk mundur, D untuk maju)
    char gearText[2];

    if (velocity < -0.01f) {
        sprintf(gearText, "R");
    } else {
        sprintf(gearText, "D");
    }

    // Text gear
    glPushMatrix();
    glTranslatef(cx - 205, cy - 18, 0);
    glScalef(0.22f, 0.22f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, gearText);
    glPopMatrix();

    // Hitung RPM
    int rpm = abs((int)(velocity * 120));
    char rpmText[10];
    sprintf(rpmText, "%03d", rpm);
    glColor3f(1, 1, 1);

    // Text RPM
    glPushMatrix();
    glTranslatef(cx + 160, cy - 18, 0);
    glScalef(0.18f, 0.18f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, rpmText);
    glPopMatrix();

    // Aktifkan kembali lighting dan depth
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Kembalikan proyeksi dan modelview ke keadaan semula
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Membuat model mobil utama
void drawRacecar(void) {
    glPushMatrix();

    // Posisi dan orientasi mobil
    glTranslatef(meX, meY, meZ);

    // Rotasi mobil berdasarkan sudut arah
    glRotatef(angleX, 0.0f, 1.0f, 0.0f);

    // Membalik arah mobil agar sesuai arah game
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    // Ukuran mobil
    glScalef(3.0f, 3.0f, 3.0f);

    // Menyesuaikan posisi ban mobil dengan jalanan
    glTranslatef(0.0f, 1.0f, 0.0f);
    
    // Driver
    if (fpv != 1) {

        // Badan driver
        cDkBlue();
        glPushMatrix();
        glTranslatef(0, 0.5f, 0);
        glScalef(0.6f, 1.0f, 0.6f);
        glutSolidSphere(0.5f, 16, 12);
        glPopMatrix();
        
        // Kepala driver
        cBlue();
        glPushMatrix();
        glTranslatef(0, 1.2f, 0);
        glScalef(0.5f, 0.5f, 0.5f);
        glutSolidSphere(0.5f, 16, 12);
        glPopMatrix();
    }
    
    // Transparansi untuk kaca dan lampu
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    // Komponen utama mobil
    drawUnderbody();
    drawMainBody();
    drawFrontGrille();
    drawHoodVents();
    drawHeadlightsBody();
    drawTaillights();
    drawWindows();
    drawRearSpoiler();
    drawMirrors();
    drawDoorLines();

    // Roda depan
    drawWheel(-1.88f, -0.18f, -2.30f, wheelAngle);
    drawWheel(1.88f, -0.18f, -2.30f, wheelAngle);

    // Roda belakang
    drawWheel(-1.88f, -0.18f, 2.35f, 0.0f);
    drawWheel(1.88f, -0.18f, 2.35f, 0.0f);

    glPopMatrix();

    // Update posisi lampu depan selalu mengarah sesuai arah mobil
    updateHeadlights();
}