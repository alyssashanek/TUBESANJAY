#ifndef TRACK_H
#define TRACK_H

void drawStartFinishLine();

void drawStartLight();

void drawTrack();

void drawCurvedWall(
    float cx,
    float cy,
    float cz,
    float innerRadius,
    float outerRadius,
    float startAngle,
    float endAngle
);

extern float axisBarriers[][6];
extern int axisBarriersCount;

extern float curveBarriers[][7];
extern int curveBarriersCount;

#endif