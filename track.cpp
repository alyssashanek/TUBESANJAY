#include "track.h"
#include "globals.h"
#include "helpers.h"
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/*\ --- Coordinate Arrays ---- \*/
int axisBarriersCount = 18;
float axisBarriers[][6] = { // Track along-axis barrier coordinates
    {-75, 0, -320, -200, 2.5, -325},
    {-80, 0, -112.5, -75, 2.5, -320},
    {-200, 0, -325, -205, 2.5, 200},
    {-205, 0, 200, -40, 2.5, 205},
    {-205, 0, 275, -35, 2.5, 280},
    {-275, 0, 200, -280, 2.5, -395},
    {2.5, 0, -55, -2.5, 2.5, -395},
    {80, 0, -112.5, 75, 2.5, -317.5},
    {125, 0, 200, 120, 2.5, 280},
    {160, 0, -235, 200, 2.5, -240},
    {160, 0, -240, 240, 2.5, -245},
    {160, 0, -160, 205, 2.5, -165},
    {200, 0, 195, 195, 2.5, 280},
    {200, 0, -160, 205, 2.5, 125},
    {280, 0, -160, 275, 2.5, 120},
    {240, 0, -400, -280, 2.5, -395},
    {240, 0, -322.5, 75, 2.5, -317.5},
    {240, 0, -322.5, 75, 2.5, -317.5},
};
int curveBarriersCount = 13;
float curveBarriers[][7] = { // Track curved barrier coordinates
    {80, 2.5, 280, 120, 115, 0, PI},
    {80, 2.5, 280, 45, 40, 0, PI},
    {-40, 2.5, 280, 80, 75, 3 * PI / 2, 2 * PI},
    {-200, 2.5, 200, 80, 75, PI / 2, PI},
    {240, 2.5, -320, 80, 75, 3 * PI / 2, 5 * PI / 2},
    {160, 2.5, -240, 80, 75, PI / 2, PI},
    {200, 2.5, -160, 80, 75, 3 * PI / 2, 2 * PI},
    {0, 2.5, 0, 140, 135, 1.6875 * PI, 3.3125 * PI},
    {0, 2.5, 0, 60, 55, 1.5125 * PI, 3.4875 * PI},
    {200, 2.5, 120, 80, 75, 0, PI / 2},
    {200, 2.5, 200, 80, 75, PI, 3 * PI / 2},
    {80, 2.5, -240, 5, 0, 3 * PI / 2, 2 * PI},
    {160, 2.5, -240, 5, 0, PI / 2, 3 * PI / 2},
};
float trackQuads[][5][3] = { // Track quad coordinates
    {{280, 0, -160}, {280, 0, 120}, {200, 0, 120}, {200, 0, -160}, {0, 1, 0}},
    {{200, 0, 200}, {200, 0, 280}, {120, 0, 280}, {120, 0, 200}, {0, 1, 0}},
    {{280, 0, -160}, {280, 0, 120}, {200, 0, 120}, {200, 0, -160}, {0, 1, 0}},
    {{-40, 0, 200}, {-40, 0, 280}, {-200, 0, 280}, {-200, 0, 200}, {0, 1, 0}},
    {{-200, 0, 200}, {-280, 0, 200}, {-280, 0, -400}, {-200, 0, -400}, {0, 1, 0}},
    {{-80, 0, -400}, {-80, 0, -320}, {-200, 0, -320}, {-200, 0, -400}, {0, 1, 0}},
    {{240, 0, -400}, {240, 0, -240}, {-80, 0, -240}, {-80, 0, -400}, {0, 1, 0}},
    {{200, 0, -240}, {200, 0, -160}, {160, 0, -160}, {160, 0, -240}, {0, 1, 0}},
    {{-80, 0, -240}, {-80, 0, -120}, {80, 0, -120}, {80, 0, -240}, {0, 1, 0}},
};
float trackCurves[][7] = { // Track curve coordinates
    {80, 0, 280, 120, 40, 0, PI},
    {-40, 0, 280, 80, 0, 3 * PI / 2, 2 * PI},
    {-200, 0, 200, 80, 0, PI / 2, PI},
    {240, 0, -320, 80, 0, 3 * PI / 2, 5 * PI / 2},
    {160, 0, -240, 80, 0, PI / 2, PI},
    {200, 0, -160, 80, 0, 3 * PI / 2, 2 * PI},
    {0, 0, 0, 140, 60, 0, 2 * PI},
    {200, 0, 120, 80, 0, 0, PI / 2},
    {200, 0, 200, 80, 0, PI, 3 * PI / 2},
};

void drawStartFinishLine(void) {
    int numSegments = 20;  // Number of checkered segments
    float segmentLength = (280.0f - 200.0f) / numSegments;
    float startY = -5.0f;  // Starting y-coordinate for the checkered pattern
    float stripeHeight = 5.0f;  // Height of each stripe

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int j = 0; j < 2; j++) {  // Two rows of checkered patterns
        for (int i = 0; i < numSegments; ++i) {
            // Set color: alternate between white (1, 1, 1) and black (0, 0, 0)
            if ((i + j) % 2 == 0) {
                glColor3f(1.0f, 1.0f, 1.0f); // White
            } else {
                glColor3f(0.0f, 0.0f, 0.0f); // Black
            }
            
            // Calculate the left and right x-coordinates for the current segment
            float leftX = 280.0f - segmentLength * i;
            float rightX = leftX - segmentLength;
            
            // Draw one segment of the start/finish line
            glVertex3f(leftX, 0.5f, startY + j * stripeHeight);  // Top left
            glVertex3f(rightX, 0.5f, startY + j * stripeHeight); // Top right
            glVertex3f(rightX, 0.5f, startY + stripeHeight + j * stripeHeight);  // Bottom right
            glVertex3f(leftX, 0.5f, startY + stripeHeight + j * stripeHeight);   // Bottom left
        }
    }
    glEnd(); // End drawing
    glEnable(GL_LIGHTING);
}
void drawStartLight(){
    float baseX = 252.5f;
    float baseY = 32.0f;
    float baseZ = -3.1f;
    float yIncrement = 3.0f;
    float xDecrement = 5.0f;
    int numLightsPerRow = 6;
    float radius = 1.4f;
    float colors[4][3] = {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
    
    // Main structure and base
    // Main structure and base - warna abu-abu seperti tiang billboard
    glColor3f(0.40f, 0.42f, 0.45f);
    drawCylinder(300, 40, 0, 180, 40, 0, 2);
    drawCylinder(290, 0, 0, 290, 40, 0, 2);
    drawCylinder(190, 0, 0, 190, 40, 0, 2);
    glColor3f(0, 0, 0);
    drawBoxFromCorners(225, 43, -2, 255, 30, -3);
    
    // Lights
    for (int row = 0; row < 4; row++) {
        for (int i = 0; i < numLightsPerRow; i++) {
            float currentX = baseX - i * xDecrement;
            float currentY = baseY + row * yIncrement;
            glColor3f(0.3, 0.3, 0.3);
            drawAngledSliceCylinder(currentX, currentY, -5.1); // Shade cover
            if (row <= currentLightRow) {
                GLfloat emissive[] = {colors[row][0], colors[row][1], colors[row][2], 1.0f};
                glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
            }
            glColor3fv(colors[row]); // Light color
            glNormal3f(0, 0, 1);
            drawCircleXY(currentX, currentY, baseZ, radius); // Actual light
            GLfloat nonEmissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, nonEmissive);
        }
    }
}

void setCheckerColor(int index) {
    if (index % 2 == 0) {
        glColor3f(1.0f, 1.0f, 1.0f); // putih
    } else {
        glColor3f(0.0f, 0.0f, 0.0f); // hitam
    }
}

void drawTrack(void){
    glColor3f(0.35, 0.35, 0.35);

    if(day){
        drawQuads(trackQuads, 9);
    } else {
        drawReflectiveQuads(trackQuads, 9);
    }

    drawCircles(trackCurves, 9);

    for (int i = 0; i < axisBarriersCount; i++) {
        setCheckerColor(i);

        drawBoxFromCorners(
            axisBarriers[i][0], axisBarriers[i][1], axisBarriers[i][2],
            axisBarriers[i][3], axisBarriers[i][4], axisBarriers[i][5]
        );
    }

    for (int i = 0; i < curveBarriersCount; ++i) {
        setCheckerColor(i);

        float* barrier = curveBarriers[i];

        drawCircle(
            barrier[0], barrier[1], barrier[2],
            barrier[4], barrier[3],
            barrier[5], barrier[6]
        );

        drawCurvedWall(
            barrier[0], barrier[1], barrier[2],
            barrier[4], barrier[3],
            barrier[5], barrier[6]
        );
    }
}

void drawCurvedWall(float cx, float cy, float cz, float innerRadius, float outerRadius, float startAngle, float endAngle) {
    glPushMatrix();
    glTranslatef(cx, cy, cz);

    // Inner wall
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 50; ++i) {
        float theta = startAngle + (endAngle - startAngle) * float(i) / 50.0f;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        glNormal3f(-cosTheta, 0.0f, -sinTheta);

        glVertex3f(innerRadius * cosTheta, 0.0f, sinTheta * innerRadius);
        glVertex3f(innerRadius * cosTheta, -cy,  sinTheta * innerRadius);
    }
    glEnd();

    // Outer wall
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 50; ++i) {
        float theta = startAngle + (endAngle - startAngle) * float(i) / 50.0f;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        glNormal3f(cosTheta, 0.0f, sinTheta);

        glVertex3f(outerRadius * cosTheta, 0.0f, sinTheta * outerRadius);
        glVertex3f(outerRadius * cosTheta, -cy,  sinTheta * outerRadius);
    }
    glEnd();

    glPopMatrix();
}