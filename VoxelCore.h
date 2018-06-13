//----------------------------------------------------------------------
// File: VoxelVolume.h
// Date: 2018-
// Description:
// Author: PARC
// Copyright(c): All rights reserved
//----------------------------------------------------------------------

#ifndef _voxelcore_h
#define _voxelcore_h

#include <cstdint>
#include <utility>
#include <tuple>

using BlockType = uint64_t; // holds 64 voxels

class VoxelCore {
private:
    unsigned int xDim, yDim, zDim;
    unsigned int bitsPerBlock;
    unsigned int numXBlocks;
    unsigned long long sizeInBlocks;

    BlockType* voxelArray;

    mutable unsigned long long cachedCount;
    mutable bool cachedCountIsValid;

    // these are only accurate after a call to getBoundingRangeAndCount
    std::pair<unsigned int, unsigned int> xLimits;
    std::pair<unsigned int, unsigned int> yLimits;
    std::pair<unsigned int, unsigned int> zLimits;

private:
    // block level transforms
    unsigned long long getVoxelBlockIndex(unsigned int x, unsigned int y, unsigned int z) const;

    BlockType getVoxelBlock(unsigned int x, unsigned int y, unsigned int z) const;
    bool extractBit(unsigned int x, BlockType block) const;

    unsigned int getIndexInBlock(unsigned int z) const;

    void checkCompatibility(const VoxelCore& v) const;

    unsigned int getPaddingSize() const;

    void clearPaddingBits();

    unsigned long long getSizeInBytes() const;

    template<class Operator> void dilateAndErode(VoxelCore * result, Operator op);


public:

    /** Create an empty voxel volume of the specified size */
    VoxelCore(unsigned int _rows, unsigned int _columns, unsigned int _planes);
    VoxelCore(const VoxelCore& voxels);

    ~VoxelCore();

    // properties
    unsigned int xDimension() const;
    unsigned int yDimension() const;
    unsigned int zDimension() const;

    unsigned long long size() const;

    unsigned long long count() const;

    // actions
    void clear(); // sets all voxels false

    void print();

    // transforms
    unsigned int getIndex(unsigned int x, unsigned int y, unsigned int z) const;

    std::tuple<unsigned int, unsigned int, unsigned int> getXYZ(unsigned int index) const;


    // accessors
    bool getVoxel(unsigned int index) const;
    bool getVoxel(unsigned int x, unsigned int y, unsigned int z) const;
    bool operator[](unsigned int index) const;

    void setVoxel(unsigned int index, bool value);
    void setVoxel(unsigned int x, unsigned int y, unsigned int z, bool value);


    // boolean

    bool isEqual(const VoxelCore& other) const;

    void subtract(const VoxelCore& other);
    void merge(const VoxelCore& other); // union is a keyword
    void intersect(const VoxelCore& other);
    void invert();


    void getBoundingRangeAndCount();
    void dilate(VoxelCore* v);
    void erode(VoxelCore* v);


    unsigned int getMinX() const;
    unsigned int getMaxX() const;
    unsigned int getMinY() const;
    unsigned int getMaxY() const;
    unsigned int getMinZ() const;
    unsigned int getMaxZ() const;

/*


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
*/
};

#endif //_voxelcore_h
