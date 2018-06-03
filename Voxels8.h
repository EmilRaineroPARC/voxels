//
// Created by erainero on 6/3/18.
//

#ifndef VOXELS_VOXELS8_H
#define VOXELS_VOXELS8_H

class Voxels8 {
    unsigned int rows, cols, planes;
    unsigned int size;
    unsigned char *voxels;
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
    Voxels8(unsigned int _cols, unsigned int _rows, unsigned int _planes) {
        rows = _rows;
        cols = _cols;
        planes = _planes;
        size = rows * cols * planes;

        voxels = (unsigned char *)malloc(size * sizeof(unsigned char));
        memset(voxels, 0, size * sizeof(unsigned char));
    };

    ~Voxels8() {
        free(voxels);
    }

    unsigned int bytes() {
        return rows * cols * planes;
    }

    unsigned int getCount() {
        unsigned int count = 0;
        unsigned char* v = voxels;

        for (unsigned int i = 0; i < size; i++) {
            unsigned char data1 = *v;
            if (data1 > 0)
                count++;
            v++;
        }
        return count;
    }

    void set(unsigned int x, unsigned int y, unsigned int z, unsigned char value) {
        voxels[x * (rows * cols) + y * cols + z] = value > 0;
    }

    void subtract(const Voxels8& other) {
        unsigned char* v0 = voxels;
        unsigned char* v1 = other.voxels;

        for (unsigned int i = 0; i < size; i++) {
            unsigned char data1 = *v1;
            if (data1 > 0)
                *v0 &= !(data1);
            v0++;
            v1++;
        }
    }

    Voxels8 *dilate(unsigned char region) const {

        auto* rtv = new Voxels8(rows, cols, planes);

        unsigned int colsTimesRows = cols * rows;
        unsigned char *v = voxels;
        unsigned char *v2 = rtv->voxels;

        for (unsigned int z = 0; z < planes; z++) {
            for (unsigned int y = 0; y < rows; y++) {
                for (unsigned int x = 0; x < cols; x++) {

                    if (*v != region) {
                        bool found = false;

                        // x + (y * cols) + (z * rows * cols)

                        if (x >= 1 && *(v-1) == region) {
                            found = true;
                        } else if (x + 1 < cols && *(v+1) == region) {
                            found = true;
                        } else if (y >= 1 && *(v - cols) == region) {
                            found = true;
                        } else if (y + 1 < rows && *(v + cols) == region) {
                            found = true;
                        } else if (z >= 1 && *(v - colsTimesRows) == region) {
                            found = true;
                        } else if (z + 1 < planes && *(v + colsTimesRows) == region) {
                            found = true;
                        }
                        if (found) {
                            *v2 = region;
                        }

                    } else {
                        *v2 = region;
                    }
                    v++;
                    v2++;
                }
            }
        }
        return rtv;
    }

    bool isEqual(const Voxels8&  other) {
        unsigned char *v0 = voxels;
        unsigned char *v1 = other.voxels;

        for (unsigned int i = 0; i < size; i++) {
            if (*v0 != *v1)
                return false;
            v0++;
            v1++;
        }
        return true;
    }

    void getBoundingRangeAndCount() {
        gotRange = true;
        maxx = 0;
        minx = cols;
        maxy = 0;
        miny = rows;
        maxz = 0;
        minz = planes;
        unsigned int x, y, z;
        unsigned int index = 0;
        count = 0;

        for (z = 0; z < planes; z++) {
            for (y = 0; y < rows; y++) {
                for (x = 0; x < cols; x++) {
                    if (voxels[index] != 0) {
                        if (x < minx) {
                            minx = x;
                        }

                        if (y < miny) {
                            miny = y;
                        }

                        if (z < minz) {
                            minz = z;
                        }

                        if (x > maxx) {
                            maxx = x;
                        }

                        if (y > maxy) {
                            maxy = y;
                        }

                        if (z > maxz) {
                            maxz = z;
                        }

                        count++;
                    }

                    index++;
                }
            }
        }
    }

};

#endif //VOXELS_VOXELS8_H
