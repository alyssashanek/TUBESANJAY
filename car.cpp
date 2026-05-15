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

// ── Quadric reusable ─────────────────────────────────────────────────────────
static GLUquadric* carQ = nullptr;
static GLUquadric* getCarQ() {
    if (!carQ) carQ = gluNewQuadric();
    return carQ;
}

// ── Helper box (center + scale) ───────────────────────────────────────────────
static void lb(float tx, float ty, float tz,
               float sx, float sy, float sz) {
    glPushMatrix();
    glTranslatef(tx, ty, tz);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// ── Palet warna ───────────────────────────────────────────────────────────────
static void cBlue()   { glColor3f(0.25f, 0.55f, 0.85f); }
static void cDkBlue() { glColor3f(0.15f, 0.38f, 0.65f); }
static void cLtBlue() { glColor3f(0.45f, 0.72f, 0.95f); }
static void cBlack()  { glColor3f(0.08f, 0.08f, 0.10f); }
static void cDkGray() { glColor3f(0.22f, 0.22f, 0.24f); }
static void cGray()   { glColor3f(0.42f, 0.42f, 0.45f); }
static void cLtGray() { glColor3f(0.72f, 0.72f, 0.75f); }
static void cSilver() { glColor3f(0.82f, 0.83f, 0.86f); }
static void cOrange() { glColor3f(0.85f, 0.40f, 0.10f); }
static void cAmber()  { glColor3f(0.95f, 0.78f, 0.20f); }

// ── Body utama ────────────────────────────────────────────────────────────────
static void drawMainBody() {
    cDkBlue(); lb(0, -0.18f, 0,        3.6f, 0.20f, 7.6f);
    cBlue();   lb(0,  0.26f, -0.1f,    3.5f, 0.72f, 7.4f);

    // Fender flare depan & belakang
    cBlue();
    lb(-1.92f, 0.22f, -2.2f,  0.28f, 0.55f, 1.8f);
    lb( 1.92f, 0.22f, -2.2f,  0.28f, 0.55f, 1.8f);
    lb(-1.92f, 0.22f,  2.1f,  0.28f, 0.60f, 2.0f);
    lb( 1.92f, 0.22f,  2.1f,  0.28f, 0.60f, 2.0f);

    // Kap mesin
    cLtBlue();
    glPushMatrix(); glTranslatef(0,0.63f,-2.2f); glScalef(3.4f,0.12f,3.0f); glutSolidCube(1.0f); glPopMatrix();
    cBlue();
    glPushMatrix(); glTranslatef(0,0.68f,-0.9f); glScalef(3.4f,0.10f,1.0f); glutSolidCube(1.0f); glPopMatrix();

    // Atap
    cLtBlue(); lb(0, 1.42f, 0.5f, 3.2f, 0.14f, 3.5f);
    glColor3f(0.50f, 0.75f, 0.95f);
    glPushMatrix(); glTranslatef(0,1.36f,0.5f); glScalef(3.1f,0.30f,3.3f); glutSolidSphere(0.52f,16,10); glPopMatrix();

    // Hatch belakang
    cBlue();
    glPushMatrix(); glTranslatef(0,1.05f,2.85f); glRotatef(-30,1,0,0); glScalef(3.2f,0.12f,1.4f); glutSolidCube(1.0f); glPopMatrix();
    cDkBlue(); lb(0, 0.70f, 3.5f, 3.2f, 0.70f, 0.18f);

    // Bumper
    cDkGray(); lb(0, -0.05f, -3.9f,  3.5f, 0.30f, 0.20f);
    cBlack();  lb(0, -0.25f, -3.85f, 3.3f, 0.08f, 0.30f);
    cDkGray(); lb(0,  0.05f,  3.75f, 3.4f, 0.55f, 0.22f);
}

// ── Grille depan ──────────────────────────────────────────────────────────────
static void drawFrontGrille() {
    cBlack(); lb(0, 0.10f, -3.92f, 2.6f, 0.50f, 0.10f);
    cDkGray();
    float barY[5] = {-0.10f, 0.02f, 0.14f, 0.26f, 0.38f};
    for (int i = 0; i < 5; i++)
        lb(0, 0.10f + barY[i], -3.90f, 2.5f, 0.055f, 0.12f);
    cGray(); lb(0, 0.18f, -3.93f, 0.38f, 0.22f, 0.08f);
    cBlack(); lb(0, 0.52f, -3.80f, 2.0f, 0.22f, 0.12f);
    cDkGray();
    float ub[3] = {0.0f, 0.09f, 0.18f};
    for (int i = 0; i < 3; i++)
        lb(0, 0.52f + ub[i], -3.79f, 1.9f, 0.04f, 0.10f);
}

// ── Vent kap ──────────────────────────────────────────────────────────────────
static void drawHoodVents() {
    cBlack(); lb(-0.65f, 0.70f, -2.0f, 0.70f, 0.10f, 1.0f);
    cDkGray();
    for (int i = 0; i < 4; i++) lb(-0.65f, 0.71f, -2.35f+i*0.22f, 0.65f, 0.06f, 0.06f);
    cBlack(); lb( 0.65f, 0.70f, -2.0f, 0.70f, 0.10f, 1.0f);
    cDkGray();
    for (int i = 0; i < 4; i++) lb( 0.65f, 0.71f, -2.35f+i*0.22f, 0.65f, 0.06f, 0.06f);
}

// ── Lampu depan ───────────────────────────────────────────────────────────────
static void drawHeadlightsBody() {
    cDkGray(); lb(-1.15f, 0.42f, -3.82f, 0.90f, 0.38f, 0.14f);
    glColor3f(0.80f, 0.88f, 0.95f); lb(-1.15f, 0.42f, -3.84f, 0.82f, 0.30f, 0.08f);
    cOrange();  lb(-1.15f, 0.26f, -3.84f, 0.82f, 0.08f, 0.08f);
    cAmber();   lb(-1.10f, 0.42f, -3.85f, 0.30f, 0.18f, 0.06f);

    cDkGray(); lb( 1.15f, 0.42f, -3.82f, 0.90f, 0.38f, 0.14f);
    glColor3f(0.80f, 0.88f, 0.95f); lb( 1.15f, 0.42f, -3.84f, 0.82f, 0.30f, 0.08f);
    cOrange();  lb( 1.15f, 0.26f, -3.84f, 0.82f, 0.08f, 0.08f);
    cAmber();   lb( 1.10f, 0.42f, -3.85f, 0.30f, 0.18f, 0.06f);
}

// ── Lampu belakang ────────────────────────────────────────────────────────────
static void drawTaillights() {
    cDkGray(); lb(-1.10f, 0.80f, 3.76f, 0.90f, 0.40f, 0.14f);
    glColor3f(0.85f, 0.15f, 0.10f); lb(-1.10f, 0.80f, 3.78f, 0.82f, 0.32f, 0.08f);
    cAmber(); lb(-1.10f, 0.65f, 3.78f, 0.82f, 0.08f, 0.08f);

    cDkGray(); lb( 1.10f, 0.80f, 3.76f, 0.90f, 0.40f, 0.14f);
    glColor3f(0.85f, 0.15f, 0.10f); lb( 1.10f, 0.80f, 3.78f, 0.82f, 0.32f, 0.08f);
    cAmber(); lb( 1.10f, 0.65f, 3.78f, 0.82f, 0.08f, 0.08f);
}

// ── Kaca ─────────────────────────────────────────────────────────────────────
static void drawWindows() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.05f, 0.10f, 0.20f, 0.85f);
    glPushMatrix(); glTranslatef(0,1.08f,-1.28f); glRotatef(55,1,0,0);
    glScalef(3.0f,0.10f,1.60f); glutSolidCube(1.0f); glPopMatrix();

    glColor4f(0.05f, 0.10f, 0.20f, 0.85f);
    glPushMatrix(); glTranslatef(0,1.05f,2.40f); glRotatef(-38,1,0,0);
    glScalef(3.0f,0.10f,1.30f); glutSolidCube(1.0f); glPopMatrix();

    glColor4f(0.10f, 0.18f, 0.30f, 0.75f);
    glPushMatrix(); glTranslatef(-1.80f,1.05f,-0.50f); glScalef(0.08f,0.60f,1.80f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.80f,1.05f, 1.40f); glScalef(0.08f,0.55f,1.40f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.80f,1.05f,-0.50f); glScalef(0.08f,0.60f,1.80f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.80f,1.05f, 1.40f); glScalef(0.08f,0.55f,1.40f); glutSolidCube(1.0f); glPopMatrix();

    glDisable(GL_BLEND);

    cDkGray();
    glPushMatrix(); glTranslatef(-1.62f,0.98f,-1.18f); glRotatef(-52,1,0,0);
    glScalef(0.10f,0.10f,1.35f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.62f,0.98f,-1.18f); glRotatef(-52,1,0,0);
    glScalef(0.10f,0.10f,1.35f); glutSolidCube(1.0f); glPopMatrix();

    lb(-1.60f, 1.42f, 0.5f, 0.10f, 0.10f, 3.5f);
    lb( 1.60f, 1.42f, 0.5f, 0.10f, 0.10f, 3.5f);
}

// ── Spoiler belakang ──────────────────────────────────────────────────────────
static void drawRearSpoiler() {
    cDkGray();
    lb(-1.0f, 1.55f, 2.85f, 0.12f, 0.28f, 0.12f);
    lb( 1.0f, 1.55f, 2.85f, 0.12f, 0.28f, 0.12f);
    cDkBlue();
    glPushMatrix(); glTranslatef(0,1.70f,2.85f); glRotatef(-8,1,0,0);
    glScalef(3.0f,0.14f,0.55f); glutSolidCube(1.0f); glPopMatrix();
    cBlack();
    glPushMatrix(); glTranslatef(0,1.78f,3.05f); glScalef(3.0f,0.20f,0.06f); glutSolidCube(1.0f); glPopMatrix();
    cDkGray();
    lb(-1.52f, 1.70f, 2.88f, 0.10f, 0.22f, 0.60f);
    lb( 1.52f, 1.70f, 2.88f, 0.10f, 0.22f, 0.60f);
}

// ── Spion & garis pintu ───────────────────────────────────────────────────────
static void drawMirrors() {
    cBlue();
    lb(-1.82f, 1.12f, -1.55f, 0.35f, 0.12f, 0.22f);
    lb( 1.82f, 1.12f, -1.55f, 0.35f, 0.12f, 0.22f);
    cBlack();
    lb(-1.92f, 1.12f, -1.55f, 0.08f, 0.14f, 0.22f);
    lb( 1.92f, 1.12f, -1.55f, 0.08f, 0.14f, 0.22f);
}

static void drawDoorLines() {
    cDkBlue();
    lb(-1.78f, 0.32f, 0.4f, 0.04f, 0.04f, 6.8f);
    lb( 1.78f, 0.32f, 0.4f, 0.04f, 0.04f, 6.8f);
    cLtGray();
    lb(-1.83f, 0.60f, -0.35f, 0.08f, 0.08f, 0.35f);
    lb( 1.83f, 0.60f, -0.35f, 0.08f, 0.08f, 0.35f);
    lb(-1.83f, 0.60f,  1.20f, 0.08f, 0.08f, 0.35f);
    lb( 1.83f, 0.60f,  1.20f, 0.08f, 0.08f, 0.35f);
}

// ── Underbody ─────────────────────────────────────────────────────────────────
static void drawUnderbody() {
    cBlack(); lb(0, -0.30f, 0, 3.2f, 0.08f, 7.0f);
    cDkGray();
    glPushMatrix();
    glTranslatef(-0.6f, -0.28f, 3.70f);
    glRotatef(90, 1, 0, 0);
    gluCylinder(getCarQ(), 0.09f, 0.09f, 0.30f, 12, 1);
    glPopMatrix();
    lb(0, -0.30f, -3.3f, 2.8f, 0.10f, 0.80f);
}

// ── Roda (memenuhi signature car.h) ──────────────────────────────────────────
void drawWheel(float tx, float ty, float tz, float angle) {
    glPushMatrix();
    glTranslatef(tx, ty, tz);
    glRotatef(angle, 0, 1, 0);    // sudut kemudi
    glRotatef(90.0f, 0, 1, 0);    // orientasi dasar roda

    // Ban
    glColor3f(0.09f, 0.09f, 0.09f);
    glutSolidTorus(0.30f, 0.62f, 20, 48);
    // Sidewall
    glColor3f(0.16f, 0.16f, 0.16f);
    glPushMatrix(); glScalef(1.0f, 0.06f, 1.0f);
    glutSolidTorus(0.25f, 0.62f, 12, 36); glPopMatrix();
    // Pelek luar
    cSilver(); glutSolidTorus(0.13f, 0.34f, 12, 32);
    // 5 jari-jari
    cLtGray();
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glRotatef(i * 72.0f, 0, 0, 1);
        glTranslatef(0, 0.22f, 0);
        glScalef(0.07f, 0.30f, 0.06f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    cGray();   glutSolidTorus(0.06f, 0.14f, 10, 20);
    cSilver(); glutSolidSphere(0.10f, 16, 12);
    // Kaliper rem (oranye)
    cOrange();
    glPushMatrix(); glTranslatef(0.08f, 0.32f, 0);
    glScalef(0.12f, 0.18f, 0.25f); glutSolidCube(1.0f); glPopMatrix();

    glPopMatrix();
}

// ── Headlight (fisika game, tidak berubah) ────────────────────────────────────
void updateHeadlights() {
    float rad = angleX * PI / 180.0;
    float lightDirX = sin(rad);
    float lightDirZ = cos(rad);
    GLfloat light0Pos[] = {
        meX - lightDirZ * 2 + lightDirX * 18.0f, 5.0f,
        meZ + lightDirX * 2 + lightDirZ * 18.0f, 1.0f
    };
    GLfloat light1Pos[] = {
        meX + lightDirZ * 2 + lightDirX * 18.0f, 5.0f,
        meZ - lightDirX * 2 + lightDirZ * 18.0f, 1.0f
    };
    GLfloat lightDir[] = {lightDirX, angleY, lightDirZ};
    glLightfv(GL_LIGHT1, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir);
    glLightfv(GL_LIGHT2, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDir);
}

// ── Gauge (tetap untuk kompatibilitas) ────────────────────────────────────────
void drawGaugeContent(void) {
    int mph = static_cast<int>((velocity / 3.0) * 120);
    char mphText[10];
    sprintf(mphText, "%03d", abs(mph));
    glPushMatrix(); glTranslatef(1.6,11.5,9.9); glScalef(0.015,0.015,0.015);
    glRotatef(180,0,1,0); writeStrokeString(GLUT_STROKE_ROMAN, mphText); glPopMatrix();
    glPushMatrix(); glTranslatef(0.9,13.5,9.9); glScalef(0.005,0.005,0.005);
    glRotatef(180,0,1,0); writeStrokeString(GLUT_STROKE_ROMAN, "SPEED"); glPopMatrix();
}

void drawHUDGauge() {
    if(!fpv) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1366, 0, 768);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float cx = 683.0f;
    float cy = 70.0f;

    // Left gauge fill
    glColor4f(0,0,0,0.35f);
    drawCircleXY(cx - 185, cy - 10, 0, 85);

    // Left outline
    glColor4f(1,1,1,0.22f);
    drawCircleXY(cx - 185, cy - 10, 0, 90);

    // Right gauge fill
    glColor4f(0,0,0,0.35f);
    drawCircleXY(cx + 185, cy - 10, 0, 85);

    // Right outline
    glColor4f(1,1,1,0.22f);
    drawCircleXY(cx + 185, cy - 10, 0, 90);

    // Main gauge fill
    glColor4f(0,0,0,0.45f);
    drawCircleXY(cx, cy, 0, 140);

    // Main outline
    glColor4f(0.03f, 0.08f, 0.20f, 0.9f);
    drawCircleXY(cx, cy, 0, 146);

    glDisable(GL_BLEND);

    // SPEED TEXT
    int mph = abs((int)(velocity * 40));

    char txt[16];
    sprintf(txt, "%03d", mph);

    glColor3f(1,1,1);

    glPushMatrix();
    glTranslatef(cx - 38, cy - 20, 0);
    glScalef(0.32f, 0.32f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, txt);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cx - 32, cy + 55, 0);
    glScalef(0.16f, 0.16f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, "SPEED");
    glPopMatrix();

    // GEAR TEXT
    char gearText[2];

    if (velocity < -0.01f)
        sprintf(gearText, "R");
    else
        sprintf(gearText, "D");

    glPushMatrix();
    glTranslatef(cx - 205, cy - 18, 0);
    glScalef(0.22f, 0.22f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, gearText);
    glPopMatrix();

    int rpm = abs((int)(velocity * 120));

    // Right Gauge Text (RPM)
    char rpmText[10];
    sprintf(rpmText, "%03d", rpm);

    glColor3f(1,1,1);

    glPushMatrix();
    glTranslatef(cx + 160, cy - 18, 0);
    glScalef(0.18f, 0.18f, 1);
    writeStrokeString(GLUT_STROKE_ROMAN, rpmText);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ── drawRacecar utama ─────────────────────────────────────────────────────────
void drawRacecar(void) {
    glPushMatrix();
    glTranslatef(meX, meY, meZ);
    glRotatef(angleX, 0.0f, 1.0f, 0.0f);
    // Mobil doc-7 menghadap -z; balik 180° agar menghadap +z (arah maju game)
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(3.0f, 3.0f, 3.0f);
    // Angkat sedikit agar ban menyentuh tanah (underbody di y=-0.30)
    glTranslatef(0.0f, 1.0f, 0.0f);
    
    // Driver
    if (!fpv) {
        cDkBlue();
        glPushMatrix();
        glTranslatef(0, 0.5f, 0);
        glScalef(0.6f, 1.0f, 0.6f);
        glutSolidSphere(0.5f, 16, 12); // Badan
        glPopMatrix();
        
        cBlue();
        glPushMatrix();
        glTranslatef(0, 1.2f, 0);
        glScalef(0.5f, 0.5f, 0.5f);
        glutSolidSphere(0.5f, 16, 12); // Kepala
        glPopMatrix();
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

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

    // Roda depan (dengan sudut kemudi)
    drawWheel(-1.88f, -0.18f, -2.30f,  wheelAngle);
    drawWheel( 1.88f, -0.18f, -2.30f,  wheelAngle);
    // Roda belakang
    drawWheel(-1.88f, -0.18f,  2.35f,  0.0f);
    drawWheel( 1.88f, -0.18f,  2.35f,  0.0f);

    glPopMatrix();
    updateHeadlights();
}