//
//

#ifndef VOXELS_VOXELSVECSLOW_H
#define VOXELS_VOXELSVECSLOW_H

#include <boost/dynamic_bitset.hpp>

class VoxelsVecSlow {
    unsigned int rows, cols, planes;
    unsigned int size;
    std::vector<bool> voxels;
    bool gotRange;
    unsigned int count;
    unsigned int maxx;
    unsigned int minx;
    unsigned int maxy;
    unsigned int miny;
    unsigned int maxz;
    unsigned int minz;

public:

    /** Create an empty voxel volume of the specified size */
    VoxelsVecSlow(unsigned int _cols, unsigned int _rows, unsigned int _planes)
    : rows(_rows)
    , cols(_cols)
    , planes(_planes)
    , size(rows * cols * planes)
    , voxels(size, 0) {
    };

    ~VoxelsVecSlow() {
    }

    unsigned int bytes() {
        return rows * cols * planes;
    }

    unsigned int getCount() {
        unsigned int count = 0;

        for (unsigned int i = 0; i < size; i++) {
            count += (int)voxels[i];
        }
        return count;
    }

    void set(unsigned int x, unsigned int y, unsigned int z, unsigned char value) {
        voxels[x * (rows * cols) + y * cols + z] = value > 0;
    }

    void subtract(const VoxelsVecSlow& other) {
        for (unsigned int i = 0; i < size; i++) {
            if (other.voxels[i])
                voxels[i] = 0;
        }
    }

    bool isEqual(const VoxelsVecSlow&  other) {
        for (unsigned int i = 0; i < size; i++) {
            if (voxels[i] != other.voxels[i]) {
                return false;
            }
        }
        return true;
    }

};

#endif //VOXELS_VOXELSVECSLOW_H
