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

float slantedQuads[][5][3] = { // Racecar quads coordinates
    {{-5, 5, 45}, {5, 5, 45}, {5, 10, 15}, {-5, 10, 15}, {0, 1, 0.25}}, // Front wing support
    {{-5, 15, -15}, {5, 15, -15}, {5, 10, -25}, {-5, 10, -25}, {0, 1, -0.5}}, // Behind cockpit slant
    {{5, 0, 15}, {5, 10, 15}, {15, 10, 5}, {15, 0, 5}, {1, 0, 0.5}}, // Body front left
    {{-5, 0, 15}, {-5, 10, 15}, {-15, 10, 5}, {-15, 0, 5}, {-1, 0, 0.5}}, // Body front right
    {{15, 0, -15}, {15, 10, -15}, {5, 10, -25}, {5, 0, -25}, {1, 0, -0.5}}, // Body back left
    {{-15, 0, -15}, {-15, 10, -15}, {-5, 10, -25}, {-5, 0, -25}, {-1, 0, -0.5}}, // Body back right
    {{-5, 10, -25}, {5, 10, -25}, {5, 15, -35}, {-5, 15, -35}, {0, 1, 0.5}}, // Rear wing support front
    {{-5, 10, -35}, {5, 10, -35}, {5, 15, -40}, {-5, 15, -40}, {0, 1, 0.5}}, // Rear wing support back
    {{-5, 10, -25}, {-5, 15, -35}, {-5, 15, -40}, {-5, 10, -35}, {-1, 0, 0}}, // Rear wing support right
    {{5, 10, -25}, {5, 15, -35}, {5, 15, -40}, {5, 10, -35}, {1, 0, 0}}, // Rear wing support left
};
float slantedTriangles[][4][3] = { // Racecar triangles coordinates
    {{-5, 10, 15}, {-5, 10, 5}, {-15, 10, 5}, {0, 1, 0}}, // Body front left
    {{5, 10, 15}, {5, 10, 5}, {15, 10, 5}, {0, 1, 0}}, // Body front right
    {{-5, 15, -15}, {-5, 10, -25}, {-15, 10, -15}, {-0.5, 1, -0.5}}, // Body back left
    {{5, 15, -15}, {5, 10, -25}, {15, 10, -15}, {0.5, 1, -0.5}}, // Body back right
    {{-5, 5, 45}, {-5, 5, 15}, {-5, 10, 15}, {-1, 0, 0}}, // Front wing support left
    {{5, 5, 45}, {5, 5, 15}, {5, 10, 15}, {1, 0, 0}}, // Front wing support right
};

void updateHeadlights() {
    float rad = angleX * PI / 180.0;
    float lightDirX = sin(rad);
    float lightDirZ = cos(rad);
    float headlightOffsetX = 2; // distance to the left and right from the center
    float headlightHeight = 5; // height of the headlights from the ground
    float headlightForward = 18.0; // forward offset of the headlights from the center

    // Calculate positions of the left and right headlights
    GLfloat light0Pos[] = {
        meX - lightDirZ * headlightOffsetX + lightDirX * headlightForward,
        headlightHeight,
        meZ + lightDirX * headlightOffsetX + lightDirZ * headlightForward,
        1.0
    };
    GLfloat light1Pos[] = {
        meX + lightDirZ * headlightOffsetX + lightDirX * headlightForward,
        headlightHeight,
        meZ - lightDirX * headlightOffsetX + lightDirZ * headlightForward,
        1.0
    };

    // Define downward tilt of the headlights (negative y-component)
    GLfloat lightDir[] = {lightDirX, angleY, lightDirZ};

    // Set the light properties for the left headlight
    glLightfv(GL_LIGHT1, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir);

    // Set the light properties for the right headlight
    glLightfv(GL_LIGHT2, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDir);
}

// Draw racecar wheel using torus
void drawWheel(float x, float y, float z, float angle) {
    float wheelWidth = 1.5f;
    float wheelRadius = 5.0f;
    glColor3f(0.0f, 0.0f, 0.0f);
    for(float i = 0.7; i <= 1; i += 0.05){
        glPushMatrix();
        glTranslatef(x * i, y, z + (abs(x)/x * 0.1 * (0.85 - i) * angle));
        glRotatef(90 + angle, 0.0f, 1.0f, 0.0f);
        glutSolidTorus(wheelWidth, wheelRadius, 30, 30);
        glPopMatrix();
    }
    glPushMatrix();
    glColor3f(0.75f, 0.75f, 0.75f);
    glTranslatef(x, y, z);
    glRotatef(-90 + angle, 0, 1, 0);
    drawCircleXY(0, 0, 0, 4);
    glPopMatrix();
}
void drawGaugeContent(void){
    int mph = static_cast<int>((velocity / 3.0) * 120);

    // Speed
    char mphText[10];
    sprintf(mphText, "%03d", abs(mph));
    glPushMatrix();
    glTranslatef(1.6, 11.5, 9.9);
    glScalef(0.015, 0.015, 0.015);
    glRotatef(180, 0.0, 1.0, 0.0);
    writeStrokeString(GLUT_STROKE_ROMAN, mphText);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.9, 13.5, 9.9);
    glScalef(0.005, 0.005, 0.005);
    glRotatef(180, 0.0, 1.0, 0.0);
    writeStrokeString(GLUT_STROKE_ROMAN, "SPEED");
    glPopMatrix();
    
    // Lap Time
    char timeText[10];
    sprintf(timeText, "%03d", int(currentLapTime));
    glPushMatrix();
    glTranslatef(-4, 11, 9.9);
    glScalef(0.01, 0.01, 0.01);
    glRotatef(180, 0.0, 1.0, 0.0);
    writeStrokeString(GLUT_STROKE_ROMAN, timeText);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(6, 11, 9.9);
    glScalef(0.01, 0.01, 0.01);
    glRotatef(180, 0.0, 1.0, 0.0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (velocity >= 0) ? 'D' : 'R');
    glPopMatrix();
}
void drawRacecar(void){
    glPushMatrix();
    glTranslatef(meX, 0.0f, meZ);
    glRotatef(angleX, 0.0f, 1.0f, 0.0f);
    glScalef(0.4f, 0.4f, 0.4f);
    
    // Front and rear wings
    glColor3f(0.25, 0.25, 0.25);
    drawBoxFromCorners(-15, 0, 45, 15, 5, 35); // Front wing
    drawBoxFromCorners(-15, 15, -35, 15, 20, -45); // Rear wing
    
    // Body
    glColor3f(0.8, 0.0, 0.0);
    drawBoxFromCorners(-5, 0, 15, 5, 10, -35); // Central body
    drawBoxFromCorners(-15, 0, 5, 15, 10, -15); // Fenders
    drawBoxFromCorners(-5, 10, -5, 5, 15, -15); // Behind cockpit
    drawQuads(slantedQuads, 10);
    drawTriangles(slantedTriangles, 6);
    
    // Driver
    glColor3f(0.25, 0.25, 0.25);
    glPushMatrix();
    glTranslatef(0, 10, 0);
    glutSolidSphere(5, 100, 100);
    glPopMatrix();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Gauge cluster
    glColor4f(0, 0, 0, 0.5);
    drawCircleXY(0, 10, 10.1, 5);
    drawCircleXY(5, 10, 10.1, 3);
    drawCircleXY(-5, 10, 10.1, 3);
    
    // Steering wheel
    if(fpv){
        glColor4f(0, 0, 0, 1.0);
        drawCircleXY(0, 10, 7, 5);
        glColor3f(1, 0, 0);
        drawCircleXY(0, 10.1, 6.9, 4);
    }
    
    GLfloat mat_emission[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glColor4f(0.1, 0.1, 0.1, 0.5);
    drawCircleXY(0, 10, 10.05, 4.9);
    drawCircleXY(5, 10, 10, 2.9);
    drawCircleXY(-5, 10, 10, 2.9);
    GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glDisable(GL_BLEND);
    
    if(fpv){drawGaugeContent();}
    
    // Intakes
    glColor3f(0, 0, 0);
    float intakeTriangles[][4][3] = {
        {{-5, 10, -15}, {-5, 15, -15}, {-15, 10, -15}, {0, 0, 1}}, // left
        {{5, 10, -15}, {5, 15, -15}, {15, 10, -15}, {0, 0, 1}}, // right
    };
    drawTriangles(intakeTriangles, 2);
    
    // Wheels
    drawWheel(-12.5, 5, 20, wheelAngle);    // Front left wheel
    drawWheel(12.5, 5, 20, wheelAngle);     // Front right wheel
    drawWheel(-12.5, 5, -30, 0);   // Rear left wheel
    drawWheel(12.5, 5, -30, 0);    // Rear right wheel
    drawCylinder(-12.5, 5, 20, 12.5, 5, 20, 1);
    drawCylinder(-12.5, 5, -30, 12.5, 5, -30, 1);
    glPopMatrix();
    
    updateHeadlights();
}
