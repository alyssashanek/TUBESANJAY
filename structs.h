#ifndef STRUCTS_H
#define STRUCTS_H

struct BitMapFile {
    int sizeX;
    int sizeY;
    unsigned char *data;
};

struct CloudPosition {
    float x, y, z;
};

struct CloudPart {
    float offsetX, offsetY, offsetZ, size;
};

struct Tree {
    float x, y, z;
    float trunkHeight;
    float treeHeight;

    Tree(
        float x,
        float y,
        float z,
        float trunkHeight,
        float treeHeight
    )
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->trunkHeight = trunkHeight;
        this->treeHeight = treeHeight;
    }
};

struct ConfettiParticle {
    float position[3];
    float velocity[3];
    float color[3];
    bool active;
};

#endif