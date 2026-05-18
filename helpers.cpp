#include "helpers.h"
#include "globals.h"
#include "structs.h"

#include <cmath>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/*\ ---- Helper Functions ---- \*/
// Load bmp files
BitMapFile *getBMPData(std::string filename) {
    BitMapFile *bmp = new BitMapFile;
    std::ifstream infile(filename.c_str(), std::ios::binary);
    if (!infile) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return nullptr;
    }
    unsigned int dataOffset;
    infile.seekg(10);
    infile.read((char*)&dataOffset, 4);
    infile.seekg(18);
    infile.read((char*)&bmp->sizeX, 4);
    infile.read((char*)&bmp->sizeY, 4);
    int width = bmp->sizeX;
    int height = bmp->sizeY;
    int rowPadded = (width * 3 + 3) & (~3);
    bmp->data = new unsigned char[width * height * 3];
    std::vector<unsigned char> row(rowPadded);
    infile.seekg(dataOffset);
    for (int y = 0; y < height; y++) {
        infile.read((char*)row.data(), rowPadded);
        for (int x = 0; x < width; x++) {
            int dst = ((height - 1 - y) * width + x) * 3;
            int src = x * 3;
            bmp->data[dst + 0] = row[src + 2];
            bmp->data[dst + 1] = row[src + 1];
            bmp->data[dst + 2] = row[src + 0];
        }
    }
    return bmp;
}

// Routine to draw a stroke character string.
void writeStrokeString(void *font, const char *string)
{
    const char *c;
    for (c = string; *c != '\0'; c++) glutStrokeCharacter(font, *c);
}
// Helper function to draw triangles
void drawTriangles(float triangles[][4][3], int numTriangles) {
    for (int i = 0; i < numTriangles; ++i) {
        glBegin(GL_TRIANGLES);
        glNormal3fv(triangles[i][3]); // The normal for the triangle
        for (int j = 0; j < 3; ++j) { // Each triangle has 3 vertices
            glVertex3fv(triangles[i][j]); // Pass the vertex to OpenGL
        }
        glEnd();
    }
}
// Helper function to draw quads
void drawQuads(float quads[][5][3], int numQuads) {
    for (int i = 0; i < numQuads; ++i) {
        glBegin(GL_QUADS);
        glNormal3fv(quads[i][4]);
        for (int j = 0; j < 4; ++j) { // Each quad has 4 vertices
            glVertex3fv(quads[i][j]); // Pass the vertex to OpenGL
        }
        glEnd();
    }
}
void drawReflectiveQuads(float quads[][5][3], int numQuads) {
    for (int i = 0; i < numQuads; ++i) {
        // Extract each corner point
        float* v1 = quads[i][0];
        float* v2 = quads[i][1];
        float* v3 = quads[i][2];
        float* v4 = quads[i][3];
        float* normal = quads[i][4];

        // Calculate dimensions
        float width = sqrt(pow(v2[0] - v1[0], 2) + pow(v2[1] - v1[1], 2) + pow(v2[2] - v1[2], 2));
        float height = sqrt(pow(v4[0] - v1[0], 2) + pow(v4[1] - v1[1], 2) + pow(v4[2] - v1[2], 2));

        // Determine the number of large quads across the width and height (each segment is 10 units)
        int numWidth = ceil(width / 2);
        int numHeight = ceil(height / 2);

        // Vector direction for width and height (each segment is 10 units long)
        float widthVec[3] = {(v2[0] - v1[0]) / numWidth, (v2[1] - v1[1]) / numWidth, (v2[2] - v1[2]) / numWidth};
        float heightVec[3] = {(v4[0] - v1[0]) / numHeight, (v4[1] - v1[1]) / numHeight, (v4[2] - v1[2]) / numHeight};

        // Draw each large quad
        glBegin(GL_QUADS);
        for (int w = 0; w < numWidth; ++w) {
            for (int h = 0; h < numHeight; ++h) {
                // Calculate corners of the large quad
                float base[3] = {
                    v1[0] + w * widthVec[0] + h * heightVec[0],
                    v1[1] + w * widthVec[1] + h * heightVec[1],
                    v1[2] + w * widthVec[2] + h * heightVec[2]
                };

                float nextWidth[3] = {base[0] + widthVec[0], base[1] + widthVec[1], base[2] + widthVec[2]};
                float nextHeight[3] = {base[0] + heightVec[0], base[1] + heightVec[1], base[2] + heightVec[2]};
                float diagonal[3] = {nextWidth[0] + heightVec[0], nextWidth[1] + heightVec[1], nextWidth[2] + heightVec[2]};

                glNormal3fv(normal);
                glVertex3fv(base);
                glVertex3fv(nextWidth);
                glVertex3fv(diagonal);
                glVertex3fv(nextHeight);
            }
        }
        glEnd();
    }
}
// Function to draw a triangle fan circle in the XY plane
void drawCircleXY(float centerX, float centerY, float centerZ, float radius) {
    int numSegments = 360;  // Number of segments to approximate the circle
    float angle;

    glPushMatrix();  // Save the current transformation matrix
    glTranslatef(centerX, centerY, centerZ);  // Move to the circle's center position

    glBegin(GL_TRIANGLE_FAN);  // Start drawing the circle using triangle fan
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center of the circle

    for (int i = 0; i <= numSegments; i++) {  // Loop through circle segments
        angle = 2.0f * M_PI * i / numSegments;  // Calculate angle
        glVertex3f(cos(angle) * radius, sin(angle) * radius, 0.0f);  // Calculate x, y from angle
    }

    glEnd();  // End drawing of circle
    glPopMatrix();  // Restore the previous transformation matrix
}
// Helper function to draw triangle strip circles, partial circles, washers in the XZ plane
void drawCircle(float cx, float cy, float cz, float innerRadius, float outerRadius, float startAngle, float endAngle) {
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glNormal3f(0, 1, 0);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 50; ++i) {
        float theta = startAngle + (endAngle - startAngle) * float(i) / float(50); // Current angle
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);
        // Outer vertex
        float xOuter = outerRadius * cosTheta;
        float zOuter = outerRadius * sinTheta;
        glVertex3f(xOuter, 0.0f, zOuter); // Output vertex for outer radius
        // Inner vertex
        float xInner = innerRadius * cosTheta;
        float zInner = innerRadius * sinTheta;
        glVertex3f(xInner, 0.0f, zInner); // Output vertex for inner radius
    }
    glEnd();
    glPopMatrix();
}
void drawCircles(float circles[][7], int numCircles) {
    for (int i = 0; i < numCircles; ++i) {
        drawCircle(
            circles[i][0], circles[i][1], circles[i][2], // cx, cy, cz
            circles[i][3], circles[i][4], // innerRadius, outerRadius
            circles[i][5], circles[i][6]  // startAngle, endAngle
        );
    }
}

// Draw cylinder given 2 3D coordinates and a radius
void drawCylinder(float x1, float y1, float z1, float x2, float y2, float z2, float radius) {
    GLUquadric* quadric = gluNewQuadric();
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;
    float length = sqrt(dx * dx + dy * dy + dz * dz);
    glPushMatrix();
    glTranslatef(x1, y1, z1);
    // Align the cylinder to the line (x1, y1, z1) -> (x2, y2, z2)
    float angle = acos(dz/length) * 180.0 / M_PI; // Convert to degrees
    glRotatef(angle, -dy, dx, 0.0);
    gluCylinder(quadric, radius, radius, length, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(quadric);
}
// Draw sliced cylinder. Used for shade covers on stop lights
void drawAngledSliceCylinder(float x, float y, float z) {
    GLUquadric* quadric = gluNewQuadric();
    int slices = 32;
    int stacks = 16;
    float radius = 1.5; // Radius of the cylinder
    float height = 2; // Height of the cylinder

    glPushMatrix();
    glTranslatef(x, y, z);
    glEnable(GL_CLIP_PLANE0);

    // Define the clipping plane
    // Normal vector at a 45-degree angle to the XY plane
    double plane[] = {0.0, 1.0, 1.0, 0.0};
    glClipPlane(GL_CLIP_PLANE0, plane);

    // Draw the cylinder
    gluCylinder(quadric, radius, radius, height, slices, stacks);

    glDisable(GL_CLIP_PLANE0);
    gluDeleteQuadric(quadric);
    glPopMatrix();
}
// Helper function to draw a box from corners
void drawBoxFromCorners(float x1, float y1, float z1, float x2, float y2, float z2) {
    float vertices[8][3] = {
        {std::min(x1, x2), std::min(y1, y2), std::min(z1, z2)},
        {std::max(x1, x2), std::min(y1, y2), std::min(z1, z2)},
        {std::max(x1, x2), std::max(y1, y2), std::min(z1, z2)},
        {std::min(x1, x2), std::max(y1, y2), std::min(z1, z2)},
        {std::min(x1, x2), std::max(y1, y2), std::max(z1, z2)},
        {std::max(x1, x2), std::max(y1, y2), std::max(z1, z2)},
        {std::max(x1, x2), std::min(y1, y2), std::max(z1, z2)},
        {std::min(x1, x2), std::min(y1, y2), std::max(z1, z2)}
    };
    int faces[6][4] = {{0, 1, 2, 3}, {7, 6, 5, 4}, {3, 2, 5, 4}, {0, 7, 6, 1}, {7, 4, 3, 0}, {1, 6, 5, 2}};
    float normals[6][3] = {{0, 0, -1}, {0, 0, 1}, {0, 1, 0}, {0, -1, 0}, {-1, 0, 0}, {1, 0, 0}};

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) {
        glNormal3fv(normals[i]);
        for (int j = 0; j < 4; ++j) glVertex3fv(vertices[faces[i][j]]);
    }
    glEnd();
}


// Function to draw multiple boxes given an array of box corner coordinates
void drawMultipleBoxes(float boxes[][6], int numBoxes) {
    for (int i = 0; i < numBoxes; i++) {
        // Each 'boxes[i]' contains the parameters for 'drawBoxFromCorners'
        drawBoxFromCorners(boxes[i][0], boxes[i][1], boxes[i][2],
                           boxes[i][3], boxes[i][4], boxes[i][5]);
    }
}
void setOrthographicProjection() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void resetPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void glShadowProjection(GLfloat shadowMat[16], GLfloat lightPos[4], GLfloat plane[4]) {
    GLfloat dot =
        plane[0] * lightPos[0] +
        plane[1] * lightPos[1] +
        plane[2] * lightPos[2] +
        plane[3] * lightPos[3];

    shadowMat[0]  = dot - lightPos[0] * plane[0];
    shadowMat[4]  = 0.f - lightPos[0] * plane[1];
    shadowMat[8]  = 0.f - lightPos[0] * plane[2];
    shadowMat[12] = 0.f - lightPos[0] * plane[3];

    shadowMat[1]  = 0.f - lightPos[1] * plane[0];
    shadowMat[5]  = dot - lightPos[1] * plane[1];
    shadowMat[9]  = 0.f - lightPos[1] * plane[2];
    shadowMat[13] = 0.f - lightPos[1] * plane[3];

    shadowMat[2]  = 0.f - lightPos[2] * plane[0];
    shadowMat[6]  = 0.f - lightPos[2] * plane[1];
    shadowMat[10] = dot - lightPos[2] * plane[2];
    shadowMat[14] = 0.f - lightPos[2] * plane[3];

    shadowMat[3]  = 0.f - lightPos[3] * plane[0];
    shadowMat[7]  = 0.f - lightPos[3] * plane[1];
    shadowMat[11] = 0.f - lightPos[3] * plane[2];
    shadowMat[15] = dot - lightPos[3] * plane[3];
}
/*\ -------------------------- \*/