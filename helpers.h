#ifndef HELPERS_H
#define HELPERS_H

#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "structs.h"

// load file bmp buat texture
BitMapFile *getBMPData(std::string filename);

// gambar box dari 2 titik sudut
void drawBoxFromCorners(float x1, float y1, float z1,float x2, float y2, float z2);

// gambar silinder dari 2 titik
void drawCylinder(float x1,float y1,float z1, float x2,float y2,float z2, float radius);

// gambar lingkaran di bidang XY
void drawCircleXY(float x, float y, float z, float radius);

// gambar beberapa quad
void drawQuads(float quads[][5][3], int numQuads);

// gambar beberapa triangle
void drawTriangles(float triangles[][4][3], int numTriangles);

// gambar quad reflektif
void drawReflectiveQuads(float quads[][5][3], int numQuads);

// gambar banyak lingkaran
void drawCircles(float circles[][7], int count);

// gambar lingkaran / ring
void drawCircle(float cx, float cy, float cz, float innerRadius, float outerRadius, float startAngle, float endAngle);

// gambar banyak box sekaligus
void drawMultipleBoxes(float boxes[][6], int count);

// gambar silinder terpotong
void drawAngledSliceCylinder(float x,float y,float z);

// render text stroke
void writeStrokeString(void *font, const char *string);

// ubah projection jadi orthographic
void setOrthographicProjection();

// balikin projection ke perspective
void resetPerspectiveProjection();

// hitung shadow projection matrix
void glShadowProjection(GLfloat shadowMat[16], GLfloat lightPos[4], GLfloat plane[4]);

#endif