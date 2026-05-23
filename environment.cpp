#include "environment.h"
#include "globals.h"
#include "helpers.h"

#include <ctime>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// Posisi awan
CloudPosition cloudPositions[] = {
    {5.0, 5.0, 10.0},
    {-15.0, 7.0, 5.0},
    {10.0, 6.0, -10.0},
    {-20.0, 8.0, -5.0},
    {15.0, 5.0, 15.0},
    {-25.0, 7.0, 20.0}
};

// Bagian pembentuk awan
CloudPart cloudParts[] = {
    {0.0, 0.0, 0.0, 0.5}, // Bola utama
    {0.6, 0.0, 0.3, 0.3}, // Bagian kanan depan
    {-0.6, 0.0, 0.3, 0.4}, // Bagian kiri depan
    {1.2, 0.0, -0.3, 0.3}, // Bagian kanan luar
    {-1.2, 0.0, -0.3, 0.35}, // Bagian kiri luar
    {0.0, 0.0, -0.6, 0.4}, // Bagian belakang tengah
    {0.6, 0.0, -0.9, 0.25}, // Bagian belakang kanan
    {-0.6, 0.0, -0.9, 0.3} // Bagian belakang kiri
};

// Data pohon
std::vector<Tree> trees = {
    {253, 0, 196, 5.6, 10.3},
    {230, 0, 326, 6.7, 9.5},
    {174, 0, 87, 7.3, 12.3},
    {160, 0, 120, 6.2, 10.7},
    {156, 0, -50, 7.1, 12.6},
    {80, 0, 280, 8.4, 12.5},
    {10, 0, 180, 9.5, 14.3},
    {-90, 8, 360, 5.6, 13.1},
    {-140, 0, 310, 7, 12},
    {-160, 0, 160, 5.6, 13.1},
    {-150, 0, 36, 7.2, 12.2},
    {-185, 0, -86, 7.1, 12.5},
    {-150, 0, -175, 7.8, 12.3}
};

// Titik pembentuk bukit
GLfloat hillPoints[4][4][3] = {
    {{-1.5, 0.0, -1.5}, {-0.5, 0.0, -1.5}, {0.5, 0.0, -1.5}, {1.5, 0.0, -1.5}},
    {{-1.5, 0.0, -0.5}, {-0.5, 1.0, -0.5}, {0.5, 1.0, -0.5}, {1.5, 0.0, -0.5}},
    {{-1.5, 0.0, 0.5}, {-0.5, 1.0, 0.5}, {0.5, 1.0, 0.5}, {1.5, 0.0, 0.5}},
    {{-1.5, 0.0, 1.5}, {-0.5, 0.0, 1.5}, {0.5, 0.0, 1.5}, {1.5, 0.0, 1.5}}
};

// Membuat permukaan rumput
void drawGrass(void) {

    // Ukuran area rumput
    float extent = 10000.0f;
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureGrass[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glNormal3f(0.0, 1.0, 0.0);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); 
        glVertex3f(-extent, -0.10f, -extent);
        glTexCoord2f(0.0, 100.0); 
        glVertex3f(-extent, -0.10f, extent);
        glTexCoord2f(100.0, 100.0); 
        glVertex3f(extent, -0.10f, extent);
        glTexCoord2f(100.0, 0.0); 
        glVertex3f(extent, -0.10f, -extent);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// Membuat awan
void drawCloud(float x, float y, float z) {

    // Warna awan siang atau malam
    float cloudShade = day ? 0.9 : 0.2;
    glColor3f(cloudShade, cloudShade, cloudShade);
    glPushMatrix();
    glTranslatef(x, y, z);

    // Bagian pembentuk awan
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glTranslatef(cloudParts[i].offsetX, cloudParts[i].offsetY, cloudParts[i].offsetZ);
        glutSolidSphere(cloudParts[i].size, 20, 20);
        glPopMatrix();
    }

    glPopMatrix();
}

// Menampilkan semua awan
void drawClouds() {
    glDisable(GL_LIGHTING);

    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glScalef(40, 40, 40);
        drawCloud(cloudPositions[i].x, cloudPositions[i].y, cloudPositions[i].z);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}

// Membuat bukit
void drawHill(float x, float y, float z, float scale) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureGrass[0]);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
    glTranslatef(x, y, z);

    // Ukuran bukit
    glScalef(scale, scale, scale); 

    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j <= 3; j++) {
            glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &hillPoints[0][0][0]);
            glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 8, 2, &hillPoints[0][0][0]);
            glEnable(GL_MAP2_TEXTURE_COORD_2);
            glEnable(GL_MAP2_VERTEX_3);
            glMapGrid2f(10, 0.0, 1.0, 20, 0.0, 1.0);
            glEvalMesh2(GL_FILL, 0, 20, 0, 20);
        }
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Membuat satu pohon
void drawTree(float x, float y, float z, float trunkHeight, float foliageRadius) {
    
    // Ukuran batang pohon
    float trunkRadius = 3;

    // Tinggi daun pohon
    float foliageHeight = 20.0f;

    glPushMatrix();

    // Posisi pohon
    glTranslatef(x, y, z);

    // Batang pohon
    glColor3f(0.55f, 0.27f, 0.07f);
    drawCylinder(0, 0, 0, 0, trunkHeight, 0, trunkRadius);

    // Daun pohon
    glColor3f(0.0f, 0.4f, 0.0f);

    for(int i = 0; i < 3; i++){
        glPushMatrix();
        glTranslatef(0.0f, trunkHeight + (i * trunkHeight), 0.0f);
        glRotatef(-90, 1, 0, 0);
        GLUquadric* quad = gluNewQuadric();
        gluCylinder(quad, foliageRadius - (i * 2.5), 0.0f, foliageHeight - (i * 2.5), 20, 20);
        gluDeleteQuadric(quad);
        glPopMatrix();
    }

    glPopMatrix();
}

// Menampilkan semua pohon
void drawTrees() {

    // Pohon default
    for (const auto& tree : trees) {
        drawTree(tree.x, tree.y, tree.z, tree.trunkHeight, tree.treeHeight);
    }
    
    static bool initialized = false;

    // generate pohon random sekali
    if (!initialized) {
        srand(time(nullptr));
        for (int i = 0; i < 50; ++i) {  // Generate 50 pohon
            float x, z;
            do {
                x = rand() % 1220 - 610;  // Range dari -610 sampai 610
                z = rand() % 1640 - 820;  // Range dari -820 sampai 820
            } while ((x > -290 && x < 330) && (z > -420 && z < 420));

            float trunkHeight = rand() % 4 + 6; // Tinggi batang pohon random antara 6 dan 10
            float treeHeight = trunkHeight + (rand() % 5 + 5); // Total tinggi batang pohon

            trees.emplace_back(x, 0, z, trunkHeight, treeHeight);
        }

        initialized = true;
    }

    // Render semua pohon
    for (const auto& tree : trees) {
        drawTree(tree.x, tree.y, tree.z, tree.trunkHeight, tree.treeHeight);
    }
}

// Membuat matahari
void drawSun() {
    glPushMatrix();
    float sunRadius = 20.0f;

    // Posisi matahari
    glTranslatef(400, 300, 1000);
    glColor3f(1.0f, 0.95f, 0.7f);

    // Efek cahaya matahari
    GLfloat mat_emission[] = {1.0f, 0.9f, 0.1f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    GLUquadric* quadric = gluNewQuadric();
    gluSphere(quadric, sunRadius, 30, 30);
    gluDeleteQuadric(quadric);

    // Reset emission
    GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glPopMatrix();
}

// Membuat bulan
void drawMoon() {
    glPushMatrix();
    float moonRadius = 10.0f;

    // Posisi bulan
    glTranslatef(-400, 300, -1000);
    glColor3f(0.95f, 0.95f, 0.95f);

    // Efek cahaya bulan
    GLfloat mat_emission[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    GLUquadric* quadric = gluNewQuadric();
    gluSphere(quadric, moonRadius, 30, 30); // Draw sphere
    gluDeleteQuadric(quadric);

    // Reset emission
    GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glPopMatrix();
}

// Inisialisasi partikel confetti
void initConfetti(ConfettiParticle confetti[], float posX, float posY, float posZ) {
    for (int i = 0; i < MAX_CONFETTI; i++) {

        // Posisi awal confetti
        confetti[i].position[0] = posX;
        confetti[i].position[1] = posY;
        confetti[i].position[2] = posZ;

        // Kecepatan gerak confetti
        confetti[i].velocity[0] = (rand() % 20 - 10) * 0.05;
        confetti[i].velocity[1] = (rand() % 30 + 10) * 0.25;
        confetti[i].velocity[2] = (rand() % 20 - 10) * 0.05;

        // Warna random confetti
        confetti[i].color[0] = rand() / (float)RAND_MAX;
        confetti[i].color[1] = rand() / (float)RAND_MAX;
        confetti[i].color[2] = rand() / (float)RAND_MAX;

        // Status confetti aktif
        confetti[i].active = true;
    }
}

// Update dan menampilkan confetti
void updateAndDrawConfetti(ConfettiParticle confetti[]) {
    
    // Ukuran partikel confetti
    glPointSize(10.0);

    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);

    for (int i = 0; i < MAX_CONFETTI; i++) {

        // Update saat confetti aktif
        if (confetti[i].active) {

            // Update posisi confetti
            confetti[i].position[0] += confetti[i].velocity[0];
            confetti[i].position[1] += confetti[i].velocity[1];
            confetti[i].position[2] += confetti[i].velocity[2];

            // Efek gravitasi
            confetti[i].velocity[1] -= 0.1;

            // Render confetti
            glColor3fv(confetti[i].color);
            glVertex3fv(confetti[i].position);

            // Nonaktifkan confetti saat jatuh ke bawah
            if (confetti[i].position[1] < -1) {
                confetti[i].active = false;
            }
        }
    }

    glEnd();
    glEnable(GL_LIGHTING);
}

// Membuat piala
void drawTrophy(void) {
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT); 

    // Efek pantulan cahaya emas
    GLfloat gold_specular[] = {1.0f, 0.95f, 0.60f, 1.0f};

    // Tingkat kilap material
    GLfloat gold_shininess[] = {80.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, gold_shininess);

    // Posisi dan rotasi piala
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(trophyRotationAngle, 0, 1, 0);

    // Quadric object
    GLUquadric* quad = gluNewQuadric();

    // Normal smoothing
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Base piala
    glPushMatrix();
    glRotatef(-90, 1, 0, 0); 
    glColor3f(0.85f, 0.65f, 0.15f); // Gold
    gluCylinder(quad, 20.0, 16.0, 10.0, 32, 1);
    glTranslatef(0, 0, 10.0);
    gluDisk(quad, 0, 16.0, 32, 1);
    glPopMatrix();

    // Leher piala
    glPushMatrix();
    glTranslatef(0.0f, 10.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glColor3f(1.00f, 0.85f, 0.25f); // Gold lebih terang
    gluCylinder(quad, 12.0, 5.0, 25.0, 32, 1);
    glPopMatrix();

    // Mangkuk bawah
    glPushMatrix();
    glTranslatef(0.0f, 35.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.95f, 0.80f, 0.20f); 
    gluCylinder(quad, 5.0, 30.0, 20.0, 32, 1);
    glPopMatrix();

    // Mangkuk atas
    glPushMatrix();
    glTranslatef(0.0f, 55.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glColor3f(1.00f, 0.85f, 0.25f); 
    gluCylinder(quad, 30.0, 32.0, 25.0, 32, 1);
    glPopMatrix();

    // Bagian dalam mangkuk
    glPushMatrix();
    glTranslatef(0.0f, 55.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.40f, 0.30f, 0.05f); 
    gluCylinder(quad, 29.5, 31.5, 25.5, 32, 1); 
    gluDisk(quad, 0, 29.5, 32, 1);
    glPopMatrix();

    // Aksen cincin dekorasi
    glColor3f(0.95f, 0.80f, 0.20f); 
    
    glPushMatrix();
    glTranslatef(0.0f, 65.0f, 0.0f);
    glRotatef(45, 1, 0, 0);
    glRotatef(30, 0, 1, 0);
    glutSolidTorus(1.5, 31.0, 16, 32);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 65.0f, 0.0f);
    glRotatef(-45, 1, 0, 0);
    glRotatef(60, 0, 1, 0);
    glutSolidTorus(1.5, 31.0, 16, 32);
    glPopMatrix();

    // Hapus quadric
    gluDeleteQuadric(quad);
    
    glPopAttrib(); 
    glPopMatrix();
}

// Membuat billboard
void drawBillboard(void) {

    // Posisi cahaya
    GLfloat lightPos[4];
    glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Permukaan tanah untuk bayangan
    GLfloat groundPlane[] = {0.0f, 1.0f, 0.0f, 0.0f};

    // Matrix bayangan
    GLfloat shadowMat[16];
    glShadowProjection(shadowMat, lightPos, groundPlane);

    // Posisi dan ukuran billboard
    float bx     = 325.0f;
    float bz     = -60.0f;
    float poleH  = 18.0f;
    float boardW = 50.0f;
    float boardH = 30.0f;
    float halfW  = boardW * 0.5f;
    float boardY = poleH;

    // Tiang billboard
    glColor3f(0.40f, 0.42f, 0.45f);
    drawCylinder(bx + 2.0f, 0, bz - halfW * 0.55f,
                 bx + 2.0f, poleH + boardH + 1.5f, 
                 bz - halfW * 0.55f, 1.8f);
    drawCylinder(bx + 2.0f, 0, bz + halfW * 0.55f,
                 bx + 2.0f, poleH + boardH + 1.5f, 
                 bz + halfW * 0.55f, 1.8f);

    // Frame horizontal billboard
    drawCylinder(bx + 2.0f, boardY, bz - halfW * 0.55f,
                 bx + 2.0f, boardY, bz + halfW * 0.55f, 0.9f);
    drawCylinder(bx + 2.0f, boardY + boardH, bz - halfW * 0.55f,
                 bx + 2.0f, boardY + boardH, bz + halfW * 0.55f, 0.9f);

    // Papan billboard
    glColor3f(0.12f, 0.12f, 0.14f);
    glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(bx + 0.6f, boardY, bz - halfW);
        glVertex3f(bx + 0.6f, boardY, bz + halfW);
        glVertex3f(bx + 0.6f, boardY + boardH, bz + halfW);
        glVertex3f(bx + 0.6f, boardY + boardH, bz - halfW);
    glEnd();

    // Bingkai billboard
    glColor3f(0.20f, 0.20f, 0.22f);
    drawBoxFromCorners(bx + 0.1f, boardY - 1.2f, bz - halfW - 1.2f,
                       bx + 0.6f, boardY + boardH + 1.2f, bz - halfW);
    drawBoxFromCorners(bx + 0.1f, boardY - 1.2f, bz + halfW,
                       bx + 0.6f, boardY + boardH + 1.2f, bz + halfW + 1.2f);
    drawBoxFromCorners(bx + 0.1f, boardY - 1.2f, bz - halfW,
                       bx + 0.6f, boardY, bz + halfW);
    drawBoxFromCorners(bx + 0.1f, boardY + boardH, bz - halfW,
                       bx + 0.6f, boardY + boardH + 1.2f, bz + halfW);

    // Tekstur billboard
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureBillboard[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(bx, boardY, bz + halfW);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(bx, boardY, bz - halfW);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(bx, boardY + boardH, bz - halfW);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(bx, boardY + boardH, bz + halfW);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    // Bayangan billboard
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);

    // Menghindari z-fighting dengan tanah
    glTranslatef(0.0f, 0.05f, 0.0f);
    glMultMatrixf(shadowMat);

    // Bayangan tiang
    drawCylinder(bx + 2.0f, 0, bz - halfW * 0.55f,
                 bx + 2.0f, poleH + boardH + 1.5f,
                 bz - halfW * 0.55f, 1.8f);

    drawCylinder(bx + 2.0f, 0, bz + halfW * 0.55f,
                 bx + 2.0f, poleH + boardH + 1.5f,
                 bz + halfW * 0.55f, 1.8f);

    // Bayangan papan billboard
    drawBoxFromCorners(bx + 0.6f, boardY, bz - halfW,
                       bx + 2.5f, boardY + boardH, bz + halfW);

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// Tekstur rumput
void loadGrassTexture() {
    BitMapFile *image[1];
    image[0] = getBMPData("textures/smallgrass.bmp");
    
    // Generate tekstur
    glGenTextures(1, textureGrass);
    glBindTexture(GL_TEXTURE_2D, textureGrass[0]);

    // Mengatur texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Mengatur filler texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Memasukkan tekstur ke OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

    // Cek error OpenGL
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << gluErrorString(err) << std::endl;
    }
}

// Generate angka random dalam range tertentu
float randomFloatInRange(float min, float max) {
    return min + ((float)rand() / (float)(RAND_MAX)) * (max - min);
}

// Tekstur billboard
void loadBillboardTexture() {
    BitMapFile *image[1];
    image[0] = getBMPData("textures/billboard.bmp");

    // Generate tekstur
    glGenTextures(1, textureBillboard);
    glBindTexture(GL_TEXTURE_2D, textureBillboard[0]);

    // Mengatur alignment texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Mengatur texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Mengatur filler texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Memasukkan tekstur ke OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

    // Cek error OpenGL
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << gluErrorString(err) << std::endl;
    }
}