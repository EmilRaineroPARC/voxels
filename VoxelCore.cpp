//----------------------------------------------------------------------
// File: VoxelCore.h
// Date: 2018-
// Description:
// Author: PARC
// Copyright(c): All rights reserved
//----------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include <cassert>

#include "VoxelCore.h"

////////// Public

VoxelCore::VoxelCore(unsigned int xDim, unsigned int yDim, unsigned int zDim)
        : m_xDim(xDim)
        , m_yDim(yDim)
        , m_zDim(zDim)
        , m_bitsPerBlock(sizeof(BlockType) * 8)
        , m_numBlocksInXDim((m_xDim + m_bitsPerBlock - 1) / m_bitsPerBlock)
        , m_sizeInBlocks(m_numBlocksInXDim * m_yDim * m_zDim)
        , m_voxelArray(new uint64_t[m_sizeInBlocks])
        , m_xLimits(0, 0)
        , m_yLimits(0, 0)
        , m_zLimits(0, 0) {

    assert(m_xDim > 0);
    assert(m_yDim > 0);
    assert(m_zDim > 0);

    clear();
};

VoxelCore::VoxelCore(const VoxelCore& other) : VoxelCore(other.m_xDim, other.m_yDim, other.m_zDim)
{
    clone(other);
}

VoxelCore::~VoxelCore() {
    if (m_voxelArray) {
        delete[] m_voxelArray;
        m_voxelArray = nullptr;
    }
}

void VoxelCore::clone(const VoxelCore& other){
    checkCompatibility(other);

    m_xLimits = other.m_xLimits;
    m_yLimits = other.m_yLimits;
    m_zLimits = other.m_zLimits;

    memcpy(m_voxelArray, other.m_voxelArray, other.getSizeInBytes());

};


unsigned int VoxelCore::xDimension() const { return m_xDim; }
unsigned int VoxelCore::yDimension() const { return m_yDim; }
unsigned int VoxelCore::zDimension() const { return m_zDim; }

unsigned int VoxelCore::size() const {
    return m_xDim * m_yDim * m_zDim;
}

void VoxelCore::clear() {
    std::memset(m_voxelArray, 0, getSizeInBytes());
}

void VoxelCore::print() {
    std::string divider(m_xDim, '-');
    std::cout << divider << std::endl;
    for (int z = static_cast<int>(m_zDim)-1; z >= 0; z--) {
        for (int y = static_cast<int>(m_yDim)-1; y >= 0; y--) {
            for (unsigned int x = 0; x < m_xDim; x++) {
                bool vox = getVoxel(x, static_cast<unsigned int>(y), static_cast<unsigned int>(z));
                std::cout << (vox?"✖":"•");
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
}

// __has_builtin doesn't compile in linux, ok on mac.  both support _builtin_popcountll
unsigned int VoxelCore::count() const {
    unsigned int count = 0;
    BlockType* v = m_voxelArray;

    for (unsigned int i = 0; i < m_sizeInBlocks; i++) {
//#if __has_builtin(__builtin_popcountll)
//            count += __builtin_popcountll(*v);
//#else
        BlockType block = *v;
        for (int b = 0; (block > 0) && (b < m_bitsPerBlock); b++) {
            if ((block & static_cast<BlockType>(1u)) > 0)
                count++;
            block = block >> 1u;
        }
//#endif
        v++;
    }

    return count;
}

unsigned int VoxelCore::getIndex(unsigned int x, unsigned int y,unsigned int z) const {
    assert(x < m_xDim);
    assert(y < m_yDim);
    assert(z < m_zDim);
    return x + (y * m_xDim) + (z * m_xDim * m_yDim);
}

std::tuple<unsigned int, unsigned int, unsigned int> VoxelCore::getXYZ(unsigned int index) const {
    assert(index < size());
    unsigned int x = index % m_xDim;
    unsigned int y = ((index - x) / m_xDim) % m_yDim;
    unsigned int z = (index - x - y * m_xDim) / (m_xDim * m_yDim);
    return std::make_tuple(x, y, z);
};

// this returns coordinates in a Eigen::Vector3i, which uses signed ints.  This is useful for
// doing coordinate arithmetic but it can overflow.
//Eigen::Vector3i VoxelCore::getCoordinates(unsigned int index) const {
//    assert(index < size());
//    auto xyz = getXYZ(index);
//    return Eigen::Vector3i(static_cast<int>(std::get<0>(xyz)),
//                           static_cast<int>(std::get<1>(xyz)),
//                           static_cast<int>(std::get<2>(xyz)));
//};

unsigned int VoxelCore::getSizeInBytes() const {
    return m_sizeInBlocks * m_bitsPerBlock/8;
}


//////// Private

inline unsigned int VoxelCore::getVoxelBlockIndex(unsigned int x, unsigned int y, unsigned int z) const {
    assert(x < m_xDim);
    assert(y < m_yDim);
    assert(z < m_zDim);
    return x / m_bitsPerBlock + (y * m_numBlocksInXDim) + (z * m_numBlocksInXDim * m_yDim);
}

inline unsigned int VoxelCore::getVoxelBlockIndex(unsigned int index) const {
    assert(index < size());
    unsigned int x = index % m_xDim;
    unsigned int yz = (index - x) / m_xDim;
    return (x / m_bitsPerBlock) + (yz * m_numBlocksInXDim);
}

inline BlockType VoxelCore::getVoxelBlock(unsigned int x, unsigned int y, unsigned int z) const {
    assert(x < m_xDim);
    assert(y < m_yDim);
    assert(z < m_zDim);
    unsigned int offset = getVoxelBlockIndex(x,y,z);
    return *(m_voxelArray+offset);
}

inline BlockType VoxelCore::getVoxelBlock(unsigned int index) const {
    assert(index < size());
    unsigned int offset = getVoxelBlockIndex(index);
    return *(m_voxelArray+offset);
}

inline bool VoxelCore::extractBit(unsigned int x, BlockType block) const {
    return ((block >> getIndexInBlock(x)) & static_cast<BlockType>(1UL)) > 0;
}


inline unsigned int VoxelCore::getIndexInBlock(unsigned int x) const {
    return (m_bitsPerBlock - 1) - (x % m_bitsPerBlock);
}


//////////Public


bool VoxelCore::getVoxel(unsigned int index) const {
    assert(index < size());

    BlockType block = getVoxelBlock(index);
    return extractBit(index % m_xDim, block);
}

bool VoxelCore::getVoxel(unsigned int x, unsigned int y, unsigned int z) const {
    assert(x < m_xDim);
    assert(y < m_yDim);
    assert(z < m_zDim);

    return getVoxel(getIndex(x,y,z));
}


bool VoxelCore::operator[](unsigned int index) const {
    return getVoxel(index);
}


// this function uses signed ints, should not be used for values that would overflow a signed int
//bool VoxelCore::getVoxel(Eigen::Vector3i coordinate) const {
//    assert(coordinate[0] >= 0 && coordinate[0] < m_xDim);
//    assert(coordinate[1] >= 0 && coordinate[1] < m_yDim);
//    assert(coordinate[2] >= 0 && coordinate[2] < m_zDim);
//    return getVoxel(static_cast<unsigned int>(coordinate[0]),
//                    static_cast<unsigned int>(coordinate[1]),
//                    static_cast<unsigned int>(coordinate[2]));
//}

void VoxelCore::setVoxel(unsigned int index, bool value) {
    assert(index < size());

    auto newBit = static_cast<BlockType>(1) << getIndexInBlock(index % m_xDim);
    if (value) {
        *(m_voxelArray + getVoxelBlockIndex(index)) |= newBit;
    } else {
        *(m_voxelArray + getVoxelBlockIndex(index)) &= ~newBit;
    }
}

void VoxelCore::setVoxel(unsigned int x, unsigned int y, unsigned int z, bool value) {
    assert(x < m_xDim);
    assert(y < m_yDim);
    assert(z < m_zDim);

    return setVoxel(getIndex(x,y,z), value);
}

//void VoxelCore::setVoxel(Eigen::Vector3i coordinate, bool value) {
//    assert(coordinate[0] >= 0 && coordinate[0] < m_xDim);
//    assert(coordinate[1] >= 0 && coordinate[1] < m_yDim);
//    assert(coordinate[2] >= 0 && coordinate[2] < m_zDim);
//    setVoxel(static_cast<unsigned int>(coordinate[0]),
//             static_cast<unsigned int>(coordinate[1]),
//             static_cast<unsigned int>(coordinate[2]), value);
//}


unsigned int VoxelCore::getMinX() const {
    return m_xLimits.first;
}

unsigned int VoxelCore::getMaxX() const {
    return m_xLimits.second;
}

unsigned int VoxelCore::getMinY() const {
    return m_yLimits.first;
}

unsigned int VoxelCore::getMaxY() const {
    return m_yLimits.second;
}

unsigned int VoxelCore::getMinZ() const {
    return m_zLimits.first;
}

unsigned int VoxelCore::getMaxZ() const {
    return m_zLimits.second;
}

void VoxelCore::setBounds(unsigned int minX, unsigned int maxX, unsigned int minY, unsigned int maxY, unsigned int minZ, unsigned int maxZ) {
    this->m_xLimits.first = minX;
    this->m_xLimits.second = maxX;

    this->m_yLimits.first = minY;
    this->m_yLimits.second = maxY;

    this->m_zLimits.first = minZ;
    this->m_zLimits.second = maxZ;
}

// boolean operations

bool VoxelCore::isEqual(const VoxelCore& other) const {
    checkCompatibility(other);

    BlockType* v0 = m_voxelArray;
    BlockType* v1 = other.m_voxelArray;

    for (unsigned int i = 0; i < m_sizeInBlocks; i++) {
        if (*v0 != *v1) {
            return false;
        }
        v0++;
        v1++;
    }
    return true;
}

unsigned int VoxelCore::getPaddingSize() const{
    return (m_bitsPerBlock - m_xDim % m_bitsPerBlock) % m_bitsPerBlock;
}

void VoxelCore::operationSubtract(const VoxelCore& other) {
    checkCompatibility(other);

    BlockType* v0 = m_voxelArray;
    BlockType* v1 = other.m_voxelArray;

    for (unsigned int i = 0; i < m_sizeInBlocks; i++) {
        BlockType data1 = *v1;
        if (*v0 > 0 && data1 > 0) {
            *v0 &= ~data1;
        }
        v0++;
        v1++;
    }
}


void VoxelCore::operationUnion(const VoxelCore& other) {
    checkCompatibility(other);

    BlockType* v0 = m_voxelArray;
    BlockType* v1 = other.m_voxelArray;

    for (unsigned int i = 0; i < m_sizeInBlocks; i++) {
        BlockType data1 = *v1;
        if (data1 > 0) {
            *v0 |= data1;
        }
        v0++;
        v1++;
    }
}

void VoxelCore::operationIntersect(const VoxelCore& other) {
    checkCompatibility(other);

    BlockType* v0 = m_voxelArray;
    BlockType* v1 = other.m_voxelArray;

    for (unsigned int i = 0; i < m_sizeInBlocks; i++) {
        BlockType data1 = *v1;
        if (*v0 > 0 || data1 > 0) {
            *v0 &= data1;
        }
        v0++;
        v1++;
    }
}

void VoxelCore::operationInvert() {
    BlockType* v0 = m_voxelArray;
    for (unsigned int i = 0; i < m_sizeInBlocks; i++) {
        *v0 = ~*v0;
        v0++;
    }
    clearPaddingBits();
}



void VoxelCore::operationDilate(VoxelCore* result) const {
    assert(result != this);
    checkCompatibility(*result);

    if (result->count() > 0) {
        result->clear();
    }

    const unsigned int zOffset = m_numBlocksInXDim * m_yDim;
    const unsigned int yOffset = m_numBlocksInXDim;
    BlockType* src = m_voxelArray;
    BlockType* dst = result->m_voxelArray;

    for (unsigned int z = 0; z < m_zDim; z++) {
        for (unsigned int y = 0; y < m_yDim; y++) {
            for (unsigned int x = 0; x < m_numBlocksInXDim; x++) {
                // x is scanline
                BlockType srcBlock = *src;
                BlockType dstBlock = srcBlock;

                if (dstBlock != ~static_cast<BlockType>(0)) {
                    // shift x
                    {
                        BlockType shiftedLeft = srcBlock << 1u; // add high bit of next word as well
                        if (x < m_numBlocksInXDim - 1) {
                            BlockType b = *(src + 1);
                            b = b >> (m_bitsPerBlock - 1);
                            shiftedLeft |= b;
                        }
                        dstBlock |= shiftedLeft;
                    }
                    {
                        BlockType shiftedRight = srcBlock >> 1u; // add low bit of prior word as well
                        if (x > 0) {
                            BlockType b = *(src - 1);
                            b = b << (m_bitsPerBlock - 1);
                            shiftedRight |= b;
                        }
                        dstBlock |= shiftedRight;
                    }

                    // shift y
                    if (y >= 1) {
                        dstBlock |= *(src - yOffset);
                    }
                    if (y < m_yDim - 1) {
                        dstBlock |= *(src + yOffset);
                    }

                    // shift z
                    if (z >= 1) {
                        dstBlock |= *(src - zOffset);
                    }
                    if (z < m_zDim - 1) {
                        dstBlock |= *(src + zOffset);
                    }

                    *dst = dstBlock;
                }
                src++;
                dst++;
            }
        }
    }

    result->clearPaddingBits();
}

// erode assumes the boundaries of the voxel volume are false and will erode boundaries
void VoxelCore::operationErode(VoxelCore* result) const {
    assert(result != this);
    checkCompatibility(*result);

    if (result->count() > 0) {
        result->clear();
    }

    if (this->count() == 0) {
        return; // nothing needs to be done
    }

    const unsigned int zOffset = m_numBlocksInXDim * m_yDim;
    const unsigned int yOffset = m_numBlocksInXDim;
    BlockType* src = m_voxelArray;
    BlockType* dst = result->m_voxelArray;

    for (unsigned int z = 0; z < m_zDim; z++) {
        for (unsigned int y = 0; y < m_yDim; y++) {
            for (unsigned int x = 0; x < m_numBlocksInXDim; x++) {
                // x is scanline
                BlockType srcBlock = *src;
                BlockType dstBlock = srcBlock;

                if (dstBlock != 0) { // only need to erode this block if dstBlock is non-zero

                    // shift x
                    {
                        BlockType shiftedLeft = srcBlock << 1u; // add high bit of next word as well
                        if (x < m_numBlocksInXDim - 1) {
                            BlockType b = *(src + 1);
                            b = b >> (m_bitsPerBlock - 1);
                            shiftedLeft |= b;
                        }
                        dstBlock &= shiftedLeft;
                    }
                    {
                        BlockType shiftedRight = srcBlock >> 1u; // add low bit of prior word as well
                        if (x > 0) {
                            BlockType b = *(src - 1);
                            b = b << (m_bitsPerBlock - 1);
                            shiftedRight |= b;
                        }
                        dstBlock &= shiftedRight;
                    }

                    // shift y
                    if (y >= 1) {
                        dstBlock &= *(src - yOffset);
                    }
                    if (y < m_yDim - 1) {
                        dstBlock &= *(src + yOffset);
                    }

                    // shift z
                    if (z >= 1) {
                        dstBlock &= *(src - zOffset);
                    }
                    if (z < m_zDim - 1) {
                        dstBlock &= *(src + zOffset);
                    }

                    // erode the y and z boundaries of the voxel volume
                    if (y == 0 || z == 0 || y == m_yDim - 1 || z == m_zDim - 1) {
                        dstBlock = 0;
                    }

                    *dst = dstBlock;
                }
                src++;
                dst++;
            }
        }
    }
}

unsigned int VoxelCore::getBoundingRangeAndCount() {
    BlockType* v = m_voxelArray;

    m_xLimits.first = m_xDim-1;
    m_xLimits.second = 0;
    m_yLimits.first = m_yDim-1;
    m_yLimits.second = 0;
    m_zLimits.first = m_zDim-1;
    m_zLimits.second = 0;

    const BlockType leftBitMask = static_cast<BlockType>(1) << (m_bitsPerBlock-1);

    unsigned int count = 0;
    for (unsigned int z = 0; z < m_zDim; z++) {
        for (unsigned int y = 0; y < m_yDim; y++) {
            for (unsigned int xBlock = 0; xBlock < m_numBlocksInXDim; xBlock++) {
                BlockType data1 = *v;
                if (data1 != 0) {
                    for (int i = 0; i < m_bitsPerBlock; i++) {
                        if (data1 & leftBitMask) {
                            unsigned int xBit = xBlock * m_bitsPerBlock + i;
                            if (xBit < m_xLimits.first)  m_xLimits.first = xBit;
                            if (xBit > m_xLimits.second) m_xLimits.second = xBit;

                            if (y < m_yLimits.first)  m_yLimits.first = y;
                            if (y > m_yLimits.second) m_yLimits.second = y;

                            if (z < m_zLimits.first)  m_zLimits.first = z;
                            if (z > m_zLimits.second) m_zLimits.second = z;

                            count++;
                        }
                        data1 = data1 << (BlockType) 1;
                    }
                }
                v++;
            }
        }
    }
    return count;
}


void VoxelCore::clearPaddingBits() {
    unsigned int paddingSize = getPaddingSize();
    BlockType paddingMask = (~static_cast<BlockType>(0u) >> paddingSize) << paddingSize;

    BlockType* v0 = m_voxelArray;
    for (unsigned int i = m_numBlocksInXDim-1; i < m_sizeInBlocks; i += m_numBlocksInXDim) {
        BlockType block = *(v0 + i);
        if (block) {
            *(v0 + i) &= paddingMask;
        }
    }
}

void VoxelCore::checkCompatibility(const VoxelCore& v) const {
    // for now, assume any difference in size is an error
    assert(m_xDim == v.m_xDim);
    assert(m_yDim == v.m_yDim);
    assert(m_zDim == v.m_zDim);
}
