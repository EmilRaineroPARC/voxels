//
// Created by erainero on 6/3/18.
//

#ifndef VOXELS_VOXELSPACKED_H
#define VOXELS_VOXELSPACKED_H

class VoxelsPacked {
    unsigned int rows, cols, planes;
    unsigned int planes32;
    unsigned int size;
    unsigned int words_per_plane;
    typedef unsigned long WORD;
    unsigned int bits_per_word;
    WORD *voxels;
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
    VoxelsPacked(unsigned int _cols, unsigned int _rows, unsigned int _planes) {
        rows = _rows;
        cols = _cols;
        planes = _planes;
        bits_per_word = sizeof(WORD) * 8;
        words_per_plane = (planes + bits_per_word - 1) / bits_per_word;
        size = rows * cols * words_per_plane;

        voxels = (WORD *) calloc(size, sizeof(WORD));
        memset(voxels, 0, size * sizeof(WORD));
    };

    ~VoxelsPacked() {
        free(voxels);
    }

    unsigned int bytes() {
        return rows * cols * words_per_plane;
    }

    unsigned int bitsPerWord() {
        return bits_per_word;
    }

    unsigned int getCount() {
        count = 0;
        WORD* v = voxels;

        for (unsigned int i = 0; i < size; i++) {
            WORD data1 = *v;
            if (data1 != 0) {
                for (int i = 0; i < sizeof(WORD) * 8; i++) {
                    if ((data1 & (WORD) 1) > 0)
                        count++;
                    data1 = data1 >> (WORD) 1;
                }
            }

            v++;
        }
        return count;
    }

    unsigned int get_index(unsigned int x, unsigned int y, unsigned int z) {
        return (x * words_per_plane * rows) + (y * words_per_plane) + (z / bits_per_word);
    }

    void set(unsigned int x, unsigned int y, unsigned int z, unsigned char value) {
        WORD newbit = (WORD) (value > 0);
        WORD* v = voxels;
        v = v +  get_index(x, y, z);
        WORD new_value = *v;
        unsigned long nth_bit = (bits_per_word - 1) - (z % bits_per_word);
        new_value ^= (-newbit ^ new_value) & (1UL << nth_bit);

        *v = new_value;
    }

    void getBoundingRangeAndCount() {
        WORD* v = voxels;

        maxx = 0;
        minx = cols;
        maxy = 0;
        miny = rows;
        maxz = 0;
        minz = planes;

        count = 0;
        for (unsigned int x = 0; x < cols; x++) {
            for (unsigned int y = 0; y < rows; y++) {
                for (unsigned int z = 0; z < words_per_plane; z++) {
                    WORD data1 = *v;
                    if (data1 != 0) {
                        for (int i = 0; i < sizeof(WORD) * 8; i++) {
                            if ((data1 & (WORD) 0x8000000000000000ul) != 0) {
                                count++;
                                int z1 = z * bits_per_word + i;
                                if (x < minx) {
                                    minx = x;
                                }
                                if (x > maxx) {
                                    maxx = x;
                                }

                                if (y < miny) {
                                    miny = y;
                                }
                                if (y > maxy) {
                                    maxy = y;
                                }

                                if (z1 < minz) {
                                    minz = z1;
                                }
                                if (z1 > maxz) {
                                    maxz = z1;
                                }
                            }
                            data1 = data1 << (WORD) 1;
                        }
                    }
                    v++;
                }
            }

        }
//        std::cout << minx << "," << miny << "," << minz << "  " << maxx << "," << maxy << "," << maxz << std::endl;
//        std::cout << count << std::endl;
    }

    void subtract(const VoxelsPacked& other) {
        WORD* v0 = voxels;
        WORD* v1 = other.voxels;

        for (unsigned int i = 0; i < size; i++) {
            WORD data1 = *v1;
            if (data1 > 0)
                *v0 &= ~data1;
            v0++;
            v1++;
        }
    }

    bool isEqual(const VoxelsPacked& other) {
        WORD* v0 = voxels;
        WORD* v1 = other.voxels;

        for (unsigned int i = 0; i < size; i++) {
            if (*v0 != *v1)
                return false;
            v0++;
            v1++;
        }
        return true;
    }

    VoxelsPacked *dilate(unsigned char region) {

        auto *rtv = new VoxelsPacked(rows, cols, planes);

        unsigned int colsTimesRows = words_per_plane * cols;
        WORD *v = voxels;
        WORD *v2 = rtv->voxels;

        for (unsigned int x = 0; x < cols; x++) {
            for (unsigned int y = 0; y < rows; y++) {
                for (unsigned int z = 0; z < words_per_plane; z++) {
                    // planes is scanline
                    WORD original_value = *v;
                    WORD value = original_value;

                    {
                        unsigned long shifted_right = original_value >> 1; // add low bit of prior word as well
                        if (z > 0) {
                            WORD v2 = *(v - 1);
                            v2 = v2 << (bits_per_word-1);
                            shifted_right = shifted_right | v2;
                        }
                        value |= shifted_right;
                    }
                    {
                        unsigned long shifted_left = original_value << 1; // add high bit of next word as well
                        if (z + 1 < words_per_plane) {
                            WORD v2 = *(v + 1);
                            v2 = v2 >> (bits_per_word-1);
                            shifted_left = shifted_left | v2;
                        }
                        value |= shifted_left;
                    }
                    if (y >= 1) {
                        WORD v2 = *(v - words_per_plane);
                        value |= v2;
                    }
                    if (y + 1 < rows) {
                        WORD v2 = *(v + words_per_plane);
                        value |= v2;
                    }
                    if (x >= 1) {
                        WORD v2 = *(v - colsTimesRows);
                        value |= v2;
                    }
                    if (x + 1 < cols) {
                        WORD v2 = *(v + colsTimesRows);
                        value |= v2;
                    }
                    *v2 = value;
                    v++;
                    v2++;
                }
            }
        }

        return rtv;
    }
};

#endif //VOXELS_VOXELSPACKED_H
