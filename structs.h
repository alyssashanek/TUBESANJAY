#ifndef STRUCTS_H
#define STRUCTS_H

// nyimpen data bitmap buat texture/gambar
struct BitMapFile {int sizeX; int sizeY; unsigned char *data;};

// posisi awan di world
struct CloudPosition {float x, y, z;};

// bagian kecil pembentuk awan
struct CloudPart {float offsetX, offsetY, offsetZ, size;};

// data pohon
struct Tree {float x, y, z; float trunkHeight; float treeHeight;
    // constructor buat isi data pohon
    Tree(float x, float y, float z, float trunkHeight, float treeHeight) {
        this->x = x; this->y = y; this->z = z; this->trunkHeight = trunkHeight; this->treeHeight = treeHeight;
    }
};

// confetti saat menang
struct ConfettiParticle {float position[3]; float velocity[3]; float color[3]; bool active;};

#endif