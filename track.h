#ifndef TRACK_H
#define TRACK_H

// gambar garis start dan finish
void drawStartFinishLine();

// render lampu countdown sebelum balapan
void drawStartLight();

// render seluruh bagian track
void drawTrack();

// bikin dinding tikungan melengkung
void drawCurvedWall(float cx, float cy, float cz, float innerRadius, float outerRadius, float startAngle, float endAngle);

// data koordinat pembatas lurus
extern float axisBarriers[][6];
extern int axisBarriersCount;

// data koordinat pembatas tikungan
extern float curveBarriers[][7];
extern int curveBarriersCount;

#endif