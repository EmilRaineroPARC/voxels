//
//

#ifndef VOXELS_VOXELSBITSETFAST_H
#define VOXELS_VOXELSBITSETFAST_H

#include <boost/dynamic_bitset.hpp>

class VoxelsBitsetFast {
    unsigned int rows, cols, planes;
    unsigned int size;
    boost::dynamic_bitset<> voxels;
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
    VoxelsBitsetFast(unsigned int _cols, unsigned int _rows, unsigned int _planes)
    : rows(_rows)
    , cols(_cols)
    , planes(_planes)
    , size(rows * cols * planes)
    , voxels(size, 0) {
    };

    ~VoxelsBitsetFast() {
    }

    unsigned int bitsetSize() {
        return voxels.size();
    }

    unsigned int bitsetBlock() {
        return voxels.num_blocks();
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

    void subtract(const VoxelsBitsetFast& other) {
        voxels &= ~other.voxels;
    }

    bool isEqual(const VoxelsBitsetFast&  other) {
        return voxels != other.voxels;
    }

};

#endif //VOXELS_VOXELSBITSETSLOW_H
