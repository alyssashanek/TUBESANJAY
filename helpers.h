#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include "structs.h"

BitMapFile *getBMPData(std::string filename);

void drawBoxFromCorners(
    float x1, float y1, float z1,
    float x2, float y2, float z2
);

void drawCylinder(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float radius
);

void drawCircleXY(float x, float y, float z, float radius);

void drawQuads(float quads[][5][3], int numQuads);

void drawTriangles(float triangles[][4][3], int numTriangles);

void drawReflectiveQuads(float quads[][5][3], int numQuads);

void drawCircles(float circles[][7], int count);

void drawMultipleBoxes(float boxes[][6], int count);

void drawCylinder(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float radius
);

void drawAngledSliceCylinder(float x,float y,float z);

void drawBoxFromCorners(
    float x1,float y1,float z1,
    float x2,float y2,float z2
);

void writeStrokeString(void *font, const char *string);

void setOrthographicProjection();

void resetPerspectiveProjection();

#endif