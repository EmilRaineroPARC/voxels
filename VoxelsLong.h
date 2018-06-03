//
// Created by erainero on 6/3/18.
//

#ifndef VOXELS_VOXELSLONG_H
#define VOXELS_VOXELSLONG_H

class VoxelsLong {
    unsigned int rows, cols, planes;
    unsigned int planes32;
    unsigned int size;
    unsigned int words_per_plane;
    unsigned long *voxels;

public:

    /** Create an empty voxel volume of the specified size */
    VoxelsLong(unsigned int _cols, unsigned int _rows, unsigned int _planes) {
        rows = _rows;
        cols = _cols;
        planes = _planes;
        unsigned int bits_per_unsigned_long = sizeof(unsigned long) * 8;
        words_per_plane = (planes + bits_per_unsigned_long - 1) / bits_per_unsigned_long;
        size = rows * cols * words_per_plane;

        voxels = (unsigned long *)malloc(size * sizeof(unsigned long));
        memset(voxels, 0, size * sizeof(unsigned long));
    };

    void subtract(const VoxelsLong& other) {
        unsigned long* v0 = voxels;
        unsigned long* v1 = other.voxels;

        for (unsigned int i = 0; i < size; i++) {
            unsigned long data1 = *v1;
            if (data1 > 0)
                *v0 &= !(data1);
            v0++;
            v1++;
        }
    }

    VoxelsLong *dilate(unsigned char region) const {

        auto *rtv = new VoxelsLong(rows, cols, planes);

        unsigned int colsTimesRows = cols * rows;
        unsigned long *v = voxels;
        unsigned long *v2 = rtv->voxels;

        for (unsigned int x = 0; x < cols; x++) {
            for (unsigned int y = 0; y < rows; y++) {
                for (unsigned int z = 0; z < words_per_plane; z++) {
                    // planes is scanline
                    unsigned long original_value = *v;
                    unsigned long value = original_value;

                    if (z >= 1) {
                        unsigned long shifted_right = original_value >> 1;
                        value |= shifted_right;
                    }
                    if (z + 1 < words_per_plane) {
                        unsigned long shifted_left = original_value << 1;
                        value |= shifted_left;
                    }
                    if (y >= 1)
                        value |= *(v - cols);
                    if (y + 1 < rows)
                        value |= *(v + cols);
                    if (x >= 1)
                        value |= *(v - colsTimesRows);
                    if (x + 1 < rows)
                        value |= *(v + colsTimesRows);
                    *v2 = value;
                    v++;
                    v2++;
                }
            }
        }

        return rtv;
    }
};

#endif //VOXELS_VOXELSLONG_H
