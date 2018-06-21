//----------------------------------------------------------------------
// File: VoxelCore.h
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

//#include "Eigen/StdVector"

using BlockType = uint64_t; // holds 64 voxels

class VoxelCore {
private:
    const unsigned int m_xDim, m_yDim, m_zDim;
    const unsigned int m_bitsPerBlock;
    const unsigned int m_numBlocksInXDim;
    const unsigned int m_sizeInBlocks;

    BlockType* m_voxelArray;

    // these are only accurate after a call to getBoundingRangeAndCount
    std::pair<unsigned int, unsigned int> m_xLimits;
    std::pair<unsigned int, unsigned int> m_yLimits;
    std::pair<unsigned int, unsigned int> m_zLimits;

private:
    VoxelCore() = default; // don't allow default constructor

    // block level transforms
    unsigned int getVoxelBlockIndex(unsigned int x, unsigned int y, unsigned int z) const;
    unsigned int getVoxelBlockIndex(unsigned int index) const;

    BlockType getVoxelBlock(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getVoxelBlock(unsigned int index) const;

    bool extractBit(unsigned int x, BlockType block) const;

    unsigned int getIndexInBlock(unsigned int z) const;

    void checkCompatibility(const VoxelCore& v) const;

    unsigned int getPaddingSize() const;

    void clearPaddingBits();

    unsigned int getSizeInBytes() const;

public:

    /** Create an empty voxel volume of the specified size */
    VoxelCore(unsigned int xDim, unsigned int yDim, unsigned int zDim);
    VoxelCore(const VoxelCore& voxels);

    ~VoxelCore();

    void clone(const VoxelCore& other);

    // properties
    unsigned int xDimension() const;
    unsigned int yDimension() const;
    unsigned int zDimension() const;

    unsigned int size() const;

    unsigned int count() const;

    // actions
    void clear(); // sets all voxels false

    void print();

    // transforms
    unsigned int getIndex(unsigned int x, unsigned int y, unsigned int z) const;

    std::tuple<unsigned int, unsigned int, unsigned int> getXYZ(unsigned int index) const;
//    Eigen::Vector3i getCoordinates(unsigned int index) const;


    // accessors
    bool getVoxel(unsigned int index) const;
    bool getVoxel(unsigned int x, unsigned int y, unsigned int z) const;
//    bool getVoxel(Eigen::Vector3i coordinate) const;
    bool operator[](unsigned int index) const;

    void setVoxel(unsigned int index, bool value);
    void setVoxel(unsigned int x, unsigned int y, unsigned int z, bool value);
//    void setVoxel(Eigen::Vector3i coordinate, bool value);


    // boolean

    bool isEqual(const VoxelCore& other) const;

    void operationSubtract(const VoxelCore& other);
    void operationUnion(const VoxelCore& other);
    void operationIntersect(const VoxelCore& other);
    void operationInvert();


    unsigned int getBoundingRangeAndCount();
    void operationDilate(VoxelCore* result) const;
    void operationErode(VoxelCore* result) const;


    unsigned int getMinX() const;
    unsigned int getMaxX() const;
    unsigned int getMinY() const;
    unsigned int getMaxY() const;
    unsigned int getMinZ() const;
    unsigned int getMaxZ() const;
    void setBounds(unsigned int minX, unsigned int maxX, unsigned int minY, unsigned int maxY, unsigned int minZ, unsigned int maxZ);

};

#endif //_voxelcore_h
