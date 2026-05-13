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

void setup(void)
{
    glEnable(GL_DEPTH_TEST); // Enable depth testing.
    glEnable(GL_LIGHTING);
    
    // Sunlight
    float sunlightPos[] = {0.0, 100.0, 100.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, sunlightPos);

    // Common spotlight properties
    GLfloat lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0};

    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);

    GLfloat spotCutoff[] = {60.0};
    glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutoff);
    glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, spotCutoff);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);  // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    currentLightRow = -1;
    updateLightSequence(0);
    
    initConfetti(confettiCannon1, 200.0, 0.0, 100);
    initConfetti(confettiCannon2, 280.0, 10.0, 100);
}

void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120,1,1,1000);
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
                std::cout << "Lap completed in " << lapTime << " seconds.\n";
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
    const float maxWheelAngle = 25.0f;
    const float wheelAngleStep = 5.0f;  // Adjust this to control the smoothness

    // Handling turning while moving
    if (velocity != 0 && lapStartTime) {
        float turnAdjustment = (fabs(velocity) <= 2) ?
            (turnSpeed * 0.5 * (velocity > 0 ? 1 : -1)) :
            (turnSpeed * (1.0 - 0.5 * (fabs(velocity) / maxVelocity)) * (velocity > 0 ? 1 : -1));

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
        // If not inside any box, update the position
        meZ = proposedMeZ;
        meX = proposedMeX;
    } else {
        // Collision detected, apply bounce back
        float elasticity = 0.25; // Coefficient of how much velocity is preserved after the bounce
        velocity = -velocity * elasticity; // Reverse and reduce velocity

        // Recalculate position using adjusted velocity
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
        teapotRotationAngle += 2.0f; // Increase the angle by 2 degrees each frame
        if (teapotRotationAngle > 360.0f) {
            teapotRotationAngle -= 360.0f; // Wrap around at 360 degrees
        }
    }
    glutPostRedisplay(); // Redraw the scene
    glutTimerFunc(16, update, 0); // Re-register timer for continuous updates
    
    updateCheckpoint(meX, meZ);
}

void drawScene(void)
{
    if(day) {
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    } else {
        glClearColor(0.05, 0.05, 0.25, 1.0);
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float baseCameraX, baseCameraY, baseCameraZ;
    float targetX, targetY, targetZ;

    if (!gameStarted) {
        // Camera rotates around the origin at a radius of 200
        baseCameraX = 300 * cos(cameraAngle);
        baseCameraZ = 300 * sin(cameraAngle);
        baseCameraY = 100;  // Fixed height above the origin

        targetX = 0;  // Looking at the origin
        targetY = 0;
        targetZ = 0;

        cameraAngle += 0.005;  // Increment the angle for smooth rotation
        
        setOrthographicProjection();
        renderCenteredText("OpenGL Racing Simulator");
        resetPerspectiveProjection();
    } else {
        // Standard game camera logic
        float cameraDistance = fpv ? 0.1 : 50; // Distance behind the car
        float cameraHeight = fpv ? 10 : 50;   // Height above the car
        float sideOffset = 50.0f;     // Distance to the side of the car for side views

        baseCameraX = meX - cameraDistance * sin(angleX * PI / 180);
        baseCameraZ = meZ - cameraDistance * cos(angleX * PI / 180);
        baseCameraY = meY + cameraHeight;

        targetX = meX;  // Car's current position
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
    }

    // Setup the camera
    gluLookAt(baseCameraX, baseCameraY, baseCameraZ, // Camera position
              targetX, targetY, targetZ, // Look at point
              0.0f, 1.0f, 0.0f); // Up vector

    drawGrass();
    drawHill(-200, 0, 450, 90);
    drawClouds();
    drawTrees();
    drawTrack();
    drawStartFinishLine();
    drawStartLight();
    drawTeapot();
    drawRacecar();
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

    if (currentCheckpoint > 6){
        updateAndDrawConfetti(confettiCannon1);
        updateAndDrawConfetti(confettiCannon2);
        
        char lapTimeText[100]; // Buffer for lap time text
        float lapTime = (lapEndTime - lapStartTime) / 1000.0f;
        sprintf(lapTimeText, "Lap completed in %.2f seconds.", lapTime);
        setOrthographicProjection();  // Switch to 2D projection
        drawText(lapTimeText, 10, 50);  // Draw text on the screen
        drawText("Press 'r' to restart.", 10, 70);  // Draw text on the screen
        resetPerspectiveProjection();  // Switch back to your 3D projection
    }
    if(!fpv){ // Third person view dials
        setOrthographicProjection();
        float mph = velocity * 25;
        drawMPHDial(mph); // Draw the MPH dial
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
    if(!gameStarted){
        // Post redisplay for the start window
        glutSetWindow(startWindow);
        glutPostRedisplay();
        
        // Post redisplay for the main game window
        glutSetWindow(mainWindow);
        glutPostRedisplay();
        
        // Re-register the timer callback to keep the updates coming
        glutTimerFunc(16, globalTimer, 0); // Approximately 60 times per second
    }
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
    teapotRotationAngle += 2.0f; // Increase the angle by 2 degrees each frame
    if (teapotRotationAngle > 360.0f) {
        teapotRotationAngle -= 360.0f; // Wrap around at 360 degrees
    }
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void) {
    cout << "Interaction Instructions:" << endl;
    cout << "General Controls:" << endl;
    cout << "\tESC - Exit the game." << endl;
    cout << "\tR - Reset the game state, repositioning the vehicle and resetting lap timing and velocity." << endl;
    
    cout << "Movement Controls:" << endl;
    cout << "\tW - Accelerate the vehicle. Speed increases until it reaches a maximum limit." << endl;
    cout << "\tS - Decelerate or reverse the vehicle. Speed decreases, and the vehicle can move backward." << endl;
    cout << "\tA - Steer left. Adjusts the wheel angle to the left, up to a maximum angle." << endl;
    cout << "\tD - Steer right. Adjusts the wheel angle to the right, down to a minimum angle." << endl;

    cout << "Camera and View Controls:" << endl;
    cout << "\tC - Toggle rear view (look behind)." << endl;
    cout << "\tQ - Look left without changing the direction of movement." << endl;
    cout << "\tE - Look right without changing the direction of movement." << endl;
    cout << "\tX - Toggle between first-person view (FPV) and third-person view." << endl;

    cout << "Miscellaneous Controls:" << endl;
    cout << "\tH - Cycle through different headlight modes: Auto, Off, Low, High." << endl;

    cout << "Stepping Controls (stepping through walls):" << endl;
    cout << "\tArrow Up - Move forward relative to the vehicle’s current direction." << endl;
    cout << "\tArrow Down - Move backward relative to the vehicle’s current direction." << endl;

    cout << "Steering Controls:" << endl;
    cout << "\tArrow Left - Rotate the vehicle’s view left without moving." << endl;
    cout << "\tArrow Right - Rotate the vehicle’s view right without moving." << endl;

    cout << "Mouse Controls:" << endl;
    cout << "\tLeft click start window to select options and start the game." << endl;
    cout << "\tRight click gameplay window to use popup menu and change day/night settings." << endl;
}

void switchToMainGame() {
    
    glutSetWindow(startWindow);
    glutHideWindow();

    glutSetWindow(mainWindow);
    glutShowWindow();
    
    meX=240, meY=0, meZ=-40, angleX=0, angleY = (headlightMode == 2 ? -1 : -1.25);
    currentCheckpoint = 0;
    timerRunning = false;
    lapStartTime = 0;
    velocity = 0;
    currentLightRow = -1;
    updateLightSequence(0);
    gameStarted = true;
    
    glutPostRedisplay();
}