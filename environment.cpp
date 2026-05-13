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

CloudPosition cloudPositions[] = {
    {5.0, 5.0, 10.0},
    {-15.0, 7.0, 5.0},
    {10.0, 6.0, -10.0},
    {-20.0, 8.0, -5.0},
    {15.0, 5.0, 15.0},
    {-25.0, 7.0, 20.0}
};
CloudPart cloudParts[] = {
    {0.0, 0.0, 0.0, 0.5}, // Central sphere
    {0.6, 0.0, 0.3, 0.3}, // Right-front sphere
    {-0.6, 0.0, 0.3, 0.4}, // Left-front sphere
    {1.2, 0.0, -0.3, 0.3}, // Far right sphere
    {-1.2, 0.0, -0.3, 0.35}, // Far left sphere
    {0.0, 0.0, -0.6, 0.4}, // Back central sphere
    {0.6, 0.0, -0.9, 0.25}, // Back right sphere
    {-0.6, 0.0, -0.9, 0.3} // Back left sphere
};
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
GLfloat hillPoints[4][4][3] = {
    {{-1.5, 0.0, -1.5}, {-0.5, 0.0, -1.5}, {0.5, 0.0, -1.5}, {1.5, 0.0, -1.5}},
    {{-1.5, 0.0, -0.5}, {-0.5, 1.0, -0.5}, {0.5, 1.0, -0.5}, {1.5, 0.0, -0.5}},
    {{-1.5, 0.0, 0.5}, {-0.5, 1.0, 0.5}, {0.5, 1.0, 0.5}, {1.5, 0.0, 0.5}},
    {{-1.5, 0.0, 1.5}, {-0.5, 0.0, 1.5}, {0.5, 0.0, 1.5}, {1.5, 0.0, 1.5}}
};

/*\ --- Drawing Functions ---- \*/
void drawGrass(void) {
    float extent = 10000.0f; // Large enough to cover the view
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureGrass[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glNormal3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-extent, -0.10f, -extent);
        glTexCoord2f(0.0, 100.0); glVertex3f(-extent, -0.10f, extent);
        glTexCoord2f(100.0, 100.0); glVertex3f(extent, -0.10f, extent);
        glTexCoord2f(100.0, 0.0); glVertex3f(extent, -0.10f, -extent);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void drawCloud(float x, float y, float z) {
    float cloudShade = day ? 0.9 : 0.2;
    glColor3f(cloudShade, cloudShade, cloudShade);
    glPushMatrix();
    glTranslatef(x, y, z);

    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glTranslatef(cloudParts[i].offsetX, cloudParts[i].offsetY, cloudParts[i].offsetZ);
        glutSolidSphere(cloudParts[i].size, 20, 20);
        glPopMatrix();
    }
    glPopMatrix();
}
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
void drawHill(float x, float y, float z, float scale) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureGrass[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, day ? GL_REPLACE : GL_MODULATE);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale); // Adjust scale to desired hill size

    for (int i = 0; i <= 3; i++) {
        for (int j = 0; j <= 3; j++) {
            glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &hillPoints[0][0][0]);
            glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 8, 2, &hillPoints[0][0][0]);  // Adjusted texture points
            glEnable(GL_MAP2_TEXTURE_COORD_2);
            glEnable(GL_MAP2_VERTEX_3);
            glMapGrid2f(10, 0.0, 1.0, 20, 0.0, 1.0);
            glEvalMesh2(GL_FILL, 0, 20, 0, 20);
        }
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}
void drawTree(float x, float y, float z, float trunkHeight, float foliageRadius) {
    float trunkRadius = 3;  // Radius of the trunk
    float foliageHeight = 20.0f; // Height of the foliage

    glPushMatrix();

    // Position the entire tree
    glTranslatef(x, y, z);

    // Draw the trunk using custom drawCylinder
    glColor3f(0.55f, 0.27f, 0.07f); // Brown color for the trunk
    drawCylinder(0, 0, 0, 0, trunkHeight, 0, trunkRadius);

    // Draw the foliage as a cone using gluCylinder
    glColor3f(0.0f, 0.4f, 0.0f); // Green color for the foliage
    for(int i = 0; i < 3; i++){
        glPushMatrix();
        glTranslatef(0.0f, trunkHeight + (i * trunkHeight), 0.0f); // Move to the position for the foliage
        glRotatef(-90, 1, 0, 0);
        GLUquadric* quad = gluNewQuadric();
        gluCylinder(quad, foliageRadius - (i * 2.5), 0.0f, foliageHeight - (i * 2.5), 20, 20); // Cone: large base, zero at top
        gluDeleteQuadric(quad);
        glPopMatrix();
    }

    glPopMatrix();
}
void drawTrees() {
    for (const auto& tree : trees) {
        drawTree(tree.x, tree.y, tree.z, tree.trunkHeight, tree.treeHeight);
    }
    
    static bool initialized = false;

    if (!initialized) {
        srand(time(nullptr)); // Seed random number generator
        // Generate random positions for trees
        for (int i = 0; i < 50; ++i) {  // Generate 50 trees
            float x, z;
            do {
                x = rand() % 1220 - 610;  // Range from -610 to 610
                z = rand() % 1640 - 820;  // Range from -820 to 820
            } while ((x > -290 && x < 330) && (z > -420 && z < 420));

            float trunkHeight = rand() % 4 + 6; // Random trunk height between 6 and 10
            float treeHeight = trunkHeight + (rand() % 5 + 5); // Total tree height will be trunk height plus 5 to 10

            trees.emplace_back(x, 0, z, trunkHeight, treeHeight);
        }
        initialized = true;
    }

    // Draw all trees
    for (const auto& tree : trees) {
        drawTree(tree.x, tree.y, tree.z, tree.trunkHeight, tree.treeHeight);
    }
}
void drawSun() {
    glPushMatrix();
    float sunRadius = 20.0f; // Large radius for the sun

    // Move to the position and draw the sun
    glTranslatef(400, 300, 1000); // Adjust these values based on your scene
    glColor3f(1.0f, 0.95f, 0.7f); // Sun color

    // Enable emissive material to make the sun glow
    GLfloat mat_emission[] = {0.9f, 0.8f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    // Create a quadric object to draw sphere
    GLUquadric* quadric = gluNewQuadric();
    gluSphere(quadric, sunRadius, 30, 30); // Draw sphere
    gluDeleteQuadric(quadric);

    // Reset emission material to none
    GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glPopMatrix();
}
void drawMoon() {
    glPushMatrix();
    float moonRadius = 10.0f;

    // Move to the position and draw the sun
    glTranslatef(-400, 300, -1000); // Adjust these values based on your scene
    glColor3f(0.95f, 0.95f, 0.95f); // Sun color

    // Enable emissive material to make the sun glow
    GLfloat mat_emission[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    // Create a quadric object to draw sphere
    GLUquadric* quadric = gluNewQuadric();
    gluSphere(quadric, moonRadius, 30, 30); // Draw sphere
    gluDeleteQuadric(quadric);

    // Reset emission material to none
    GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glPopMatrix();
}

void initConfetti(ConfettiParticle confetti[], float posX, float posY, float posZ) {
    for (int i = 0; i < MAX_CONFETTI; i++) {
        confetti[i].position[0] = posX;
        confetti[i].position[1] = posY;
        confetti[i].position[2] = posZ;
        confetti[i].velocity[0] = (rand() % 20 - 10) * 0.05;
        confetti[i].velocity[1] = (rand() % 30 + 10) * 0.25;
        confetti[i].velocity[2] = (rand() % 20 - 10) * 0.05;
        confetti[i].color[0] = rand() / (float)RAND_MAX;
        confetti[i].color[1] = rand() / (float)RAND_MAX;
        confetti[i].color[2] = rand() / (float)RAND_MAX;
        confetti[i].active = true;
    }
}
void updateAndDrawConfetti(ConfettiParticle confetti[]) {
    glPointSize(10.0); // Set point size for confetti particles
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    for (int i = 0; i < MAX_CONFETTI; i++) {
        if (confetti[i].active) {
            // Update position
            confetti[i].position[0] += confetti[i].velocity[0];
            confetti[i].position[1] += confetti[i].velocity[1];
            confetti[i].position[2] += confetti[i].velocity[2];

            // Apply gravity
            confetti[i].velocity[1] -= 0.1; // gravity effect

            // Draw particle
            glColor3fv(confetti[i].color);
            glVertex3fv(confetti[i].position);

            // Deactivate confetti that falls below a certain height
            if (confetti[i].position[1] < -1) {
                confetti[i].active = false;
            }
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawTeapot(void){
    glPushMatrix();
    glTranslatef(0.0f, 25.0f, 0.0f);
    glRotatef(teapotRotationAngle, 0, 1, 0);
    glColor3f(1.0f, 0.8f, 0.0f);
    glutSolidTeapot(40.0); // The parameter is the radius of the teapot
    glPopMatrix();
}

// Load grass texture
void loadGrassTexture() {
    BitMapFile *image[1];
    image[0] = getBMPData("textures/smallgrass.bmp");
    
    glGenTextures(1, textureGrass);
    glBindTexture(GL_TEXTURE_2D, textureGrass[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

    // Error check
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << gluErrorString(err) << std::endl;
    }
}
// Generate a random float in range [min, max]
float randomFloatInRange(float min, float max) {
    return min + ((float)rand() / (float)(RAND_MAX)) * (max - min);
}