//----------------------------------------------------------------------
// File: VoxelVolume.h
// Date: 2018-
// Description:
// Author: PARC
// Copyright(c): All rights reserved
//----------------------------------------------------------------------

#include "VoxelCore.h"
#include <cstring>
#include <iostream>
#include <cassert>

using ulong2 = unsigned long long;

VoxelCore::VoxelCore(unsigned int _xDim, unsigned int _yDim, unsigned int _zDim)
        : xDim(_xDim)
        , yDim(_yDim)
        , zDim(_zDim)
        , bitsPerBlock(sizeof(BlockType) * 8)
        , numXBlocks((xDim + bitsPerBlock - 1) / bitsPerBlock)
        , sizeInBlocks(numXBlocks * yDim * zDim)
        , voxelArray(new uint64_t[sizeInBlocks])
        , cachedCount(0)
        , cachedCountIsValid(true)
        , xLimits(0, 0)
        , yLimits(0, 0)
        , zLimits(0, 0) {

    assert(xDim > 0);
    assert(yDim > 0);
    assert(zDim > 0);

    clear();
};

VoxelCore::VoxelCore(const VoxelCore& other)
        : xDim(other.xDim)
        , yDim(other.yDim)
        , zDim(other.zDim)
        , bitsPerBlock(other.bitsPerBlock)
        , numXBlocks(other.numXBlocks)
        , sizeInBlocks(other.sizeInBlocks)
        , voxelArray(new uint64_t[other.sizeInBlocks])
        , cachedCount(other.cachedCount)
        , cachedCountIsValid(other.cachedCountIsValid)
        , xLimits(other.xLimits)
        , yLimits(other.yLimits)
        , zLimits(other.zLimits)
{
    memcpy(voxelArray, other.voxelArray, other.getSizeInBytes());
}

VoxelCore::~VoxelCore() {
    delete [] voxelArray;
}

////////// Public
unsigned int VoxelCore::xDimension() const { return xDim; }
unsigned int VoxelCore::yDimension() const { return yDim; }
unsigned int VoxelCore::zDimension() const { return zDim; }

unsigned long long VoxelCore::size() const {
    return static_cast<ulong2>(xDim) * yDim * zDim;
}

void VoxelCore::clear() {
    std::memset(voxelArray, 0, getSizeInBytes());
}

void VoxelCore::print() {
    std::string divider(xDim, '-');
    std::cout << divider << std::endl;
    for (int z = static_cast<int>(zDim)-1; z >= 0; z--) {
        for (int y = static_cast<int>(yDim)-1; y >= 0; y--) {
            for (unsigned int x = 0; x < xDim; x++) {
                bool vox = getVoxel(x,y,z);
                std::cout << (vox?"✖":"•");
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
}

ulong2 VoxelCore::count() const {
    if (cachedCountIsValid) {
        return cachedCount;
    } else {
        ulong2 count = 0;
        BlockType* v = voxelArray;

        for (unsigned int i = 0; i < sizeInBlocks; i++) {
            BlockType block = *v;
            for (int b = 0; block && b < bitsPerBlock; b++) {
                if ((block & static_cast<BlockType>(1)) > 0)
                    count++;
                block = block >> 1;
            }
            v++;
        }

        cachedCount = count;
        cachedCountIsValid = true;
        // use __builtin_popcount if available
        return count;
    }
}

unsigned int VoxelCore::getIndex(unsigned int x, unsigned int y,unsigned int z) const {
    assert(x < xDim);
    assert(y < yDim);
    assert(z < zDim);
    return x + (y * xDim) + (z * xDim * yDim);
}

std::tuple<unsigned int, unsigned int, unsigned int> VoxelCore::getXYZ(unsigned int index) const {
    assert(index < size());
    unsigned int x = index % xDim;
    unsigned int y = ((index - x) / xDim) % yDim;
    unsigned int z = (index - x - y * xDim) / (xDim * yDim);
    return std::make_tuple(x, y, z);
};


unsigned long long VoxelCore::getSizeInBytes() const {
    return sizeInBlocks * bitsPerBlock/8;
}


//////// Private

ulong2 VoxelCore::getVoxelBlockIndex(unsigned int x, unsigned int y, unsigned int z) const {
    assert(x < xDim);
    assert(y < yDim);
    assert(z < zDim);
    return static_cast<ulong2>(x / bitsPerBlock) + (y * numXBlocks) + (z * numXBlocks * yDim);
}

BlockType VoxelCore::getVoxelBlock(unsigned int x, unsigned int y, unsigned int z) const {
    assert(x < xDim);
    assert(y < yDim);
    assert(z < zDim);
    ulong2 offset = getVoxelBlockIndex(x,y,z);
    return *(voxelArray+offset);
}

bool VoxelCore::extractBit(unsigned int x, BlockType block) const {
    return ((block >> getIndexInBlock(x)) & static_cast<BlockType>(1UL)) > 0;
}


unsigned int VoxelCore::getIndexInBlock(unsigned int x) const {
    return (bitsPerBlock - 1) - (x % bitsPerBlock);
}


void VoxelCore::setVoxel(unsigned int index, bool value) {
    assert(index < size());

    std::tuple<unsigned int, unsigned int, unsigned int> xyz = getXYZ(index);
    setVoxel(std::get<0>(xyz), std::get<1>(xyz), std::get<2>(xyz), value);
}

void VoxelCore::setVoxel(unsigned int x, unsigned int y, unsigned int z, bool value) {
    assert(x < xDim);
    assert(y < yDim);
    assert(z < zDim);

    cachedCountIsValid = false;

    auto newBit = static_cast<BlockType>(1) << getIndexInBlock(x);
    if (value) {
        *(voxelArray + getVoxelBlockIndex(x, y, z)) |= newBit;
    } else {
        *(voxelArray + getVoxelBlockIndex(x, y, z)) &= ~newBit;
    }
}

bool VoxelCore::getVoxel(unsigned int index) const {
    std::tuple<unsigned int, unsigned int, unsigned int> xyz = getXYZ(index);
    return getVoxel(std::get<0>(xyz), std::get<1>(xyz), std::get<2>(xyz));
}

bool VoxelCore::getVoxel(unsigned int x, unsigned int y, unsigned int z) const {
    assert(x < xDim);
    assert(y < yDim);
    assert(z < zDim);

    BlockType block = getVoxelBlock(x,y,z);
    return extractBit(x, block);
}

bool VoxelCore::operator[](unsigned int index) const {
    return getVoxel(index);
}



unsigned int VoxelCore::getMinX() const {
    return xLimits.first;
}

unsigned int VoxelCore::getMaxX() const {
    return xLimits.second;
}

unsigned int VoxelCore::getMinY() const {
    return yLimits.first;
}

unsigned int VoxelCore::getMaxY() const {
    return yLimits.second;
}

unsigned int VoxelCore::getMinZ() const {
    return zLimits.first;
}

unsigned int VoxelCore::getMaxZ() const {
    return zLimits.second;
}

// boolean operations

bool VoxelCore::isEqual(const VoxelCore& other) const {
    checkCompatibility(other);

    BlockType* v0 = voxelArray;
    BlockType* v1 = other.voxelArray;

    for (unsigned int i = 0; i < sizeInBlocks; i++) {
        if (*v0 != *v1) return false;
        v0++;
        v1++;
    }
    return true;
}

unsigned int VoxelCore::getPaddingSize() const{
    return (bitsPerBlock - xDim % bitsPerBlock) % bitsPerBlock;
}

void VoxelCore::subtract(const VoxelCore& other) {
    checkCompatibility(other);
    cachedCountIsValid = false;

    BlockType* v0 = voxelArray;
    BlockType* v1 = other.voxelArray;

    for (unsigned int i = 0; i < sizeInBlocks; i++) {
        BlockType data1 = *v1;
        if (*v0 > 0 && data1 > 0)
            *v0 &= ~data1;
        v0++;
        v1++;
    }
}


void VoxelCore::merge(const VoxelCore& other) {
    checkCompatibility(other);
    cachedCountIsValid = false;

    BlockType* v0 = voxelArray;
    BlockType* v1 = other.voxelArray;

    for (unsigned int i = 0; i < sizeInBlocks; i++) {
        BlockType data1 = *v1;
        if (data1 > 0)
            *v0 |= data1;
        v0++;
        v1++;
    }
}

void VoxelCore::intersect(const VoxelCore& other) {
    checkCompatibility(other);
    cachedCountIsValid = false;

    BlockType* v0 = voxelArray;
    BlockType* v1 = other.voxelArray;

    for (unsigned int i = 0; i < sizeInBlocks; i++) {
        BlockType data1 = *v1;
        if (*v0 > 0 || data1 > 0)
            *v0 &= data1;
        v0++;
        v1++;
    }
}

void VoxelCore::invert() {
    BlockType* v0 = voxelArray;
    for (unsigned int i = 0; i < sizeInBlocks; i++) {
        *v0 = ~*v0;
        v0++;
    }
    clearPaddingBits();
    cachedCount = size() - cachedCount;
}

template void VoxelCore::dilateAndErode< std::bit_or<BlockType> >(VoxelCore * result, std::bit_or<BlockType>);
template void VoxelCore::dilateAndErode< std::bit_and<BlockType> >(VoxelCore * result, std::bit_and<BlockType>);

template<class Operator>
void VoxelCore::dilateAndErode(VoxelCore * result, Operator op) {
    assert(result != this);
    checkCompatibility(*result);

    if (result->count() > 0) {
        result->clear();
    }

    //todo: when eroding, this function erodes on the x-border, but not on the y or z.

    unsigned int zOffset = numXBlocks * yDim;
    BlockType* src = voxelArray;
    BlockType* dst = result->voxelArray;

    for (unsigned int z = 0; z < zDim; z++) {
        for (unsigned int y = 0; y < yDim; y++) {
            for (unsigned int x = 0; x < numXBlocks; x++) {
                // x is scanline
                BlockType srcBlock = *src;
                BlockType dstBlock = srcBlock;

                // shift x
                {
                    BlockType shiftedLeft = srcBlock << 1u; // add high bit of next word as well
                    if (x + 1 < numXBlocks) {
                        BlockType b = *(src + 1);
                        b = b >> (bitsPerBlock - 1);
                        shiftedLeft = op(shiftedLeft, b);
                    }
                    dstBlock = op(dstBlock, shiftedLeft);
                }
                {
                    BlockType shiftedRight = srcBlock >> 1u; // add low bit of prior word as well
                    if (x > 0) {
                        BlockType b = *(src - 1);
                        b = b << (bitsPerBlock - 1);
                        shiftedRight = op(shiftedRight, b);
                    }
                    dstBlock = op(dstBlock, shiftedRight);
                }

                // shift y
                if (y >= 1) {
                    dstBlock = op(dstBlock, *(src - numXBlocks));
                }
                if (y + 1 < yDim) {
                    dstBlock = op(dstBlock, *(src + numXBlocks));
                }

                // shift z
                if (z >= 1) {
                    dstBlock = op(dstBlock, *(src - zOffset));
                }
                if (z + 1 < zDim) {
                    dstBlock = op(dstBlock, *(src + zOffset));
                }

                *dst = dstBlock;
                src++;
                dst++;
            }
        }
    }

    // only need to do this on dilation
    result->clearPaddingBits();
}

void VoxelCore::dilate(VoxelCore* v) {
    dilateAndErode(v, std::bit_or<BlockType>());
}

void VoxelCore::erode(VoxelCore* v) {
    dilateAndErode(v, std::bit_and<BlockType>());
}

void VoxelCore::getBoundingRangeAndCount() {
    BlockType* v = voxelArray;

    xLimits.first = xDim-1;
    xLimits.second = 0;
    yLimits.first = yDim-1;
    yLimits.second = 0;
    zLimits.first = zDim-1;
    zLimits.second = 0;

    const BlockType leftBitMask = static_cast<BlockType>(1) << (bitsPerBlock-1);

    unsigned int count = 0;
    for (unsigned int z = 0; z < zDim; z++) {
        for (unsigned int y = 0; y < yDim; y++) {
            for (unsigned int xBlock = 0; xBlock < numXBlocks; xBlock++) {
                BlockType data1 = *v;
                if (data1 != 0) {
                    for (int i = 0; i < bitsPerBlock; i++) {
                        if (data1 & leftBitMask) {
                            unsigned int xBit = xBlock * bitsPerBlock + i;
                            if (xBit < xLimits.first)  xLimits.first = xBit;
                            if (xBit > xLimits.second) xLimits.second = xBit;

                            if (y < yLimits.first)  yLimits.first = y;
                            if (y > yLimits.second) yLimits.second = y;

                            if (z < zLimits.first)  zLimits.first = z;
                            if (z > zLimits.second) zLimits.second = z;

                            count++;
                        }
                        data1 = data1 << (BlockType) 1;
                    }
                }
                v++;
            }
        }

    }

    cachedCount = count;
    cachedCountIsValid = true;
}


void VoxelCore::clearPaddingBits() {
    unsigned int paddingSize = getPaddingSize();
    BlockType paddingMask = (~static_cast<BlockType>(0UL) >> paddingSize) << paddingSize;

    BlockType* v0 = voxelArray;
    for (unsigned int i = numXBlocks-1; i < sizeInBlocks; i += numXBlocks) {
        BlockType block = *(v0 + i);
        if (block) {
            *(v0 + i) &= paddingMask;
        }
    }

}

void VoxelCore::checkCompatibility(const VoxelCore& v) const {
    // for now, assume any difference in size is an error
    assert(xDim == v.xDim);
    assert(yDim == v.yDim);
    assert(zDim == v.zDim);
    assert(bitsPerBlock == v.bitsPerBlock);
}
