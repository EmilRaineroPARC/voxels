cmake_minimum_required(VERSION 3.10)
project(voxels)

set(CMAKE_CXX_STANDARD 11)

add_executable(voxels main.cpp Timer.h Voxels8.h VoxelsLong.h VoxelsPacked.h VoxelsBitsetSlow.h VoxelsBitsetFast.h VoxelsVecSlow.h)