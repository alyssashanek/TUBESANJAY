#include "game.h"

#include "globals.h"
#include "environment.h"
#include "track.h"
#include "car.h"
#include "ui.h"
#include "helpers.h"

#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;
#include <algorithm>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void setup(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);

    // SUN LIGHT POSITION
    GLfloat sunlightPos[] = {250.0f, 350.0f, 250.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sunlightPos);

    // GLOBAL AMBIENT
    GLfloat globalAmbient[] = {0.55f, 0.55f, 0.55f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // SUN DIFFUSE
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 0.95f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    // SUN SPECULAR
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // HEADLIGHTS
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);
    GLfloat spotCutoff = 60.0f;
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutoff);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotCutoff);

    // MATERIAL
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    currentLightRow = -1;
    updateLightSequence(0);

    initConfetti(confettiCannon1, 200.0, 0.0, 100);
    initConfetti(confettiCannon2, 280.0, 10.0, 100);

    loadGrassTexture();
    loadBillboardTexture();
}

void resize(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    gluPerspective(70.0, aspect, 1.0, 2000.0);

    glMatrixMode(GL_MODELVIEW);
}

void updateCheckpoint(float x, float z) {
    switch (currentCheckpoint) {
        case 0:
            if (z > 0 && !timerRunning) {
                currentCheckpoint++;
                lapStartTime = glutGet(GLUT_ELAPSED_TIME);
                timerRunning = true;
                std::cout << "Lap started!\n";
            }
            break;
        case 1:
            if (z > 280) currentCheckpoint++;
            break;
        case 2:
            if (x < -200) currentCheckpoint++;
            break;
        case 3:
            if (z < -320) currentCheckpoint++;
            break;
        case 4:
            if (x > 0 && z > 60) currentCheckpoint++;
            break;
        case 5:
            if (x > 240) currentCheckpoint++;
            break;
        case 6:
            if (z > 0) {
                currentCheckpoint++;
                timerRunning = false;
                lapEndTime = glutGet(GLUT_ELAPSED_TIME);
                float lapTime = (lapEndTime - lapStartTime) / 1000.0f;

                if (!hasBestLapTime || lapTime < bestLapTime) {
                    bestLapTime = lapTime;
                    hasBestLapTime = true;
                    std::cout << "New best lap: " << bestLapTime << " seconds.\n";
                } else {
                    std::cout << "Lap completed in " << lapTime << " seconds.\n";
                }
                currentLightRow = -1;
            }
            break;
    }
}

int isInsideAnyBox(float x, float z, float boxes[][6], int numBoxes) {
    float radius = 5.0f; // Radius of the circle around the point

    for (int i = 0; i < numBoxes; i++) {
        // Calculate expanded bounds to include the circle's radius
        float x1 = fmin(boxes[i][0], boxes[i][3]) - radius;
        float x2 = fmax(boxes[i][0], boxes[i][3]) + radius;
        float z1 = fmin(boxes[i][2], boxes[i][5]) - radius;
        float z2 = fmax(boxes[i][2], boxes[i][5]) + radius;

        // Check if the center of the circle is within the expanded bounds
        if (x >= x1 && x <= x2 && z >= z1 && z <= z2) {
            return 1; // Center of the circle is within an expanded box
        }
    }
    return 0; // Center of the circle is not inside any expanded box
}
bool isWithinAngles(float x, float z, float cx, float cz, float startAngle, float endAngle) {
    float angleToPoint = atan2(z - cz, x - cx) * 180.0 / PI;
    if (angleToPoint < 0) angleToPoint += 360;

    float normalizedStartAngle = fmod(startAngle, 360);
    float normalizedEndAngle = fmod(endAngle, 360);

    if (normalizedStartAngle <= normalizedEndAngle) {
        return angleToPoint >= normalizedStartAngle && angleToPoint <= normalizedEndAngle;
    } else {
        return angleToPoint >= normalizedStartAngle || angleToPoint <= normalizedEndAngle;
    }
}

// Function to determine if a point (x, z) intersects with any annular or partial ring barrier.
int isInsideAnyCircle(float x, float z, float circles[][7], int numCircles) {
    float carRadius = 6;
    for (int i = 0; i < numCircles; i++) {
        // Circle center and radii
        float cx = circles[i][0];
        float cz = circles[i][2];
        float outerRadius = circles[i][3] + carRadius;
        float innerRadius = circles[i][4] - carRadius;
        float startAngle = circles[i][5] * 180.0 / PI;
        float endAngle = circles[i][6] * 180.0 / PI;
        float outerRadiusSquared = outerRadius * outerRadius;
        float innerRadiusSquared = innerRadius * innerRadius;

        // Calculate squared distance from point to circle center
        float dx = x - cx;
        float dz = z - cz;
        float distSquared = dx * dx + dz * dz;

        // Check if within annular range
        if (distSquared <= outerRadiusSquared && distSquared >= innerRadiusSquared && isWithinAngles(x, z, cx, cz, startAngle, endAngle)) {
            return 1; // Collision detected
        }
    }
    return 0; // No collision detected
}

void update(int value) {
    if(day){
        glEnable(GL_LIGHT0);  // Sunlight
        if (headlightMode == 3) {
            glDisable(GL_LIGHT1); // Disable left headlight
            glDisable(GL_LIGHT2); // Disable right headlight
        }
    } else {
        glDisable(GL_LIGHT0); // Disable sunlight
        if (headlightMode == 3) {
            glEnable(GL_LIGHT1);  // Enable left headlight
            glEnable(GL_LIGHT2);  // Enable right headlight
        }
    }
    
    if (keyStates['w']) { // Accelerate
        velocity += acceleration;
        if (velocity > maxVelocity) velocity = maxVelocity;
    } else if (keyStates['s'] && lapStartTime) { // Decelerate
        velocity -= deceleration;
        if (velocity < -maxVelocity) velocity = -maxVelocity;
    } else { // Automatic deceleration when no keys are pressed
        if (velocity > 0) velocity -= deceleration;
        else if (velocity < 0) velocity += deceleration;
        if (std::abs(velocity) < deceleration) velocity = 0; // Stop completely if speed is very low
    }

    // Define the maximum wheel angle
    const float maxWheelAngle = 35.0f;
    const float wheelAngleStep = 6.5f;

    // Handling turning while moving
    if (velocity != 0 && lapStartTime) {
        float speedFactor = fabs(velocity) / maxVelocity;
    float steeringPower = 2.2f - (0.7f * speedFactor);

    if (steeringPower < 1.25f) {
        steeringPower = 1.25f;
    }

    float turnAdjustment = turnSpeed * steeringPower * (velocity > 0 ? 1 : -1);

        if (keyStates['a']) { // Turn left
            angleX += turnAdjustment;
            wheelAngle += wheelAngle < maxWheelAngle ? wheelAngleStep : 0;
            wheelAngle = std::min(wheelAngle, maxWheelAngle); // Ensure it does not exceed max angle
        } else if (keyStates['d']) { // Turn right
            angleX -= turnAdjustment;
            wheelAngle -= wheelAngle > -maxWheelAngle ? wheelAngleStep : 0;
            wheelAngle = std::max(wheelAngle, -maxWheelAngle); // Ensure it does not exceed min angle
        }
    } else {
        // If neither 'a' nor 'd' is pressed or the car is not moving, gradually return the wheel to the center
        if (wheelAngle < 0) {
            wheelAngle += wheelAngleStep;
            wheelAngle = std::min(wheelAngle, 0.0f); // Do not overshoot the center
        } else if (wheelAngle > 0) {
            wheelAngle -= wheelAngleStep;
            wheelAngle = std::max(wheelAngle, 0.0f); // Do not overshoot the center
        }
    }

    // Check if the proposed new position is within any barriers and then update position
    float proposedMeZ = meZ + velocity * cos(angleX * PI / 180);
    float proposedMeX = meX + velocity * sin(angleX * PI / 180);
    if (!isInsideAnyBox(proposedMeX, proposedMeZ, axisBarriers, axisBarriersCount) &&
        !isInsideAnyCircle(proposedMeX, proposedMeZ, curveBarriers, curveBarriersCount)) {
        meZ = proposedMeZ;
        meX = proposedMeX;
    } else {
        float elasticity = 0.25;
        velocity = -velocity * elasticity;

        meZ += velocity * cos(angleX * PI / 180);
        meX += velocity * sin(angleX * PI / 180);
    }
    
    if(!useIdleFunc){
        for (int i = 0; i < 6; i++) {
            // Update cloud position
            cloudPositions[i].z += cloudSpeed;

            // Reset cloud position if it moves too far
            if (cloudPositions[i].z > 25.0) {
                cloudPositions[i].z = -25.0;
            }
        }
            trophyRotationAngle += 2.0f;

        if (trophyRotationAngle > 360.0f) {
            trophyRotationAngle -= 360.0f;
        }
    }
    glutPostRedisplay(); // Redraw the scene
    glutTimerFunc(16, update, 0); // Re-register timer for continuous updates
    
    updateCheckpoint(meX, meZ);
}

void drawScene(void){
    if (!gameStarted) {
        drawStartScreen();
        return;
    }

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100.0, (float)w / (float)h, 1.0, 2000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(day) {
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    } else {
        glClearColor(0.05, 0.05, 0.25, 1.0);
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float baseCameraX, baseCameraY, baseCameraZ;
    float targetX, targetY, targetZ;

    float cameraDistance = fpv ? 0.1f : 90.0f;
    float cameraHeight = fpv ? 10.0f : 35.0f;
    float sideOffset = 70.0f;

    baseCameraX = meX - cameraDistance * sin(angleX * PI / 180);
    baseCameraZ = meZ - cameraDistance * cos(angleX * PI / 180);
    baseCameraY = meY + cameraHeight;

    targetX = meX;
    targetY = meY + 10;
    targetZ = meZ;

    if (lookBehind) {
        baseCameraX = meX + cameraDistance * sin(angleX * PI / 180);
        baseCameraZ = meZ + cameraDistance * cos(angleX * PI / 180);
    } else if (lookLeft) {
        baseCameraX = meX + sideOffset * cos(angleX * PI / 180);
        baseCameraZ = meZ - sideOffset * sin(angleX * PI / 180);
    } else if (lookRight) {
        baseCameraX = meX - sideOffset * cos(angleX * PI / 180);
        baseCameraZ = meZ + sideOffset * sin(angleX * PI / 180);
    }

gluLookAt(
    baseCameraX, baseCameraY, baseCameraZ,
    targetX, targetY, targetZ,
    0.0f, 1.0f, 0.0f
);

    drawGrass();
    drawBillboard();
    drawHill(-200, 0, 450, 90);
    drawClouds();
    drawTrees();
    drawTrack();
    drawStartFinishLine();
    drawStartLight();
    drawTrophy();
    drawRacecar();
    drawHUDGauge();

    if(day){drawSun();}
    else{drawMoon();}
    
    if (timerRunning) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        currentLapTime = (currentTime - lapStartTime) / 1000.0f;
        char currentLapTimeText[100];
        sprintf(currentLapTimeText, "Current Lap Time: %.2f seconds", currentLapTime);
        setOrthographicProjection();
        drawText(currentLapTimeText, 10, 50);
        resetPerspectiveProjection();
    }

    if (currentCheckpoint > 6) {
    updateAndDrawConfetti(confettiCannon1);
    updateAndDrawConfetti(confettiCannon2);

    char lapTimeText[100];
    float lapTime = (lapEndTime - lapStartTime) / 1000.0f;
    sprintf(lapTimeText, "Lap completed in %.2f seconds.", lapTime);

    setOrthographicProjection();

    int startX = 10;
    int startY = 50;
    int lineGap = 30;

    drawText(lapTimeText, startX, startY);

    char bestLapText[100];
    if (hasBestLapTime) {
        sprintf(bestLapText, "Best Lap: %.2f seconds", bestLapTime);
    } else {
        sprintf(bestLapText, "Best Lap: --");
    }

    drawText(bestLapText, startX, startY + lineGap);
    drawText("Press 'r' to restart.", startX, startY + lineGap * 2);

    resetPerspectiveProjection();
}

    glutSwapBuffers();
}

/*\ -------------------------- \*/
/*\ - Initialization Routine - \*/
void updateLightSequence(int value) {
    if (currentLightRow < 3) {
        currentLightRow++;
        glutPostRedisplay();  // Request a redraw to update the scene
        glutTimerFunc(lightUpdateTime, updateLightSequence, 0);  // Continue the timer
    }
}

void startScreenResize(int w, int h) {
    // Avoid division by zero
    if (h == 0) h = 1;

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    // Update the projection to match the new window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, w, 0.0, h);  // Map the complete window

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void globalTimer(int value) {
    glutSetWindow(mainWindow);
    glutPostRedisplay();

    glutTimerFunc(16, globalTimer, 0);
}

void idle() {
    for (int i = 0; i < 6; i++) {
        // Update cloud position
        cloudPositions[i].z += cloudSpeed;

        // Reset cloud position if it moves too far
        if (cloudPositions[i].z > 25.0) {
            cloudPositions[i].z = -25.0;
        }
    }

    trophyRotationAngle += 2.0f;

    if (trophyRotationAngle > 360.0f) {
        trophyRotationAngle -= 360.0f;
    }
    glutPostRedisplay();
}

// Routine untuk menampilkan instruksi interaksi pada window C++.
void printInteraction(void) {
    cout << "Petunjuk Kontrol Permainan:" << endl;

    cout << "Kontrol Umum:" << endl;
    cout << "\tESC - Keluar dari permainan." << endl;
    cout << "\tR - Reset permainan, mengembalikan posisi mobil serta mengatur ulang timer lap dan kecepatan." << endl;

    cout << "Kontrol Pergerakan:" << endl;
    cout << "\tW - Menambah kecepatan mobil." << endl;
    cout << "\tS - Mengurangi kecepatan atau mundur." << endl;
    cout << "\tA - Belok ke kiri." << endl;
    cout << "\tD - Belok ke kanan." << endl;

    cout << "Kontrol Kamera dan Pandangan:" << endl;
    cout << "\tC - Melihat ke belakang." << endl;
    cout << "\tQ - Melihat ke kiri tanpa mengubah arah mobil." << endl;
    cout << "\tE - Melihat ke kanan tanpa mengubah arah mobil." << endl;
    cout << "\tX - Mengganti tampilan First Person View (FPV) dan Third Person View." << endl;

    cout << "Kontrol Lampu:" << endl;
    cout << "\tH - Mengganti mode lampu mobil: Auto, Mati, Low Beam, dan High Beam." << endl;

    cout << "Kontrol Tambahan (Menembus Tembok):" << endl;
    cout << "\tPanah Atas - Bergerak maju mengikuti arah mobil." << endl;
    cout << "\tPanah Bawah - Bergerak mundur mengikuti arah mobil." << endl;

    cout << "Kontrol Arah Pandang:" << endl;
    cout << "\tPanah Kiri - Memutar arah pandang ke kiri tanpa bergerak." << endl;
    cout << "\tPanah Kanan - Memutar arah pandang ke kanan tanpa bergerak." << endl;

    cout << "Kontrol Mouse:" << endl;
    cout << "\tKlik kiri pada start window untuk memilih mode dan memulai permainan." << endl;
    cout << "\tKlik kanan saat bermain untuk membuka menu popup dan mengganti mode siang/malam." << endl;
}

void switchToMainGame() {
    glutSetWindow(mainWindow);

    meX = 240;
    meY = 0;
    meZ = -40;

    angleX = 0;
    angleY = (headlightMode == 2 ? -1 : -1.25);

    currentCheckpoint = 0;
    timerRunning = false;
    lapStartTime = 0;
    velocity = 0;

    currentLightRow = -1;
    updateLightSequence(0);

    gameStarted = true;

    glutPostRedisplay();
}