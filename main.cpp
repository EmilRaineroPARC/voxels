#include <iostream>
#include <string.h>

#include "Timer.h"
#include "Voxels8.h"
//#include "VoxelsLong.h"
#include "VoxelsPacked.h"

double TestVoxels8_Subtract(int size, int iterations) {
    Voxels8 a(size, size, size);
    Voxels8 b(size, size, size);
    Timer timer;

    for (int i=0; i< iterations; i++) {
        a.subtract(b);
    }
    return timer.elapsed();
}

double TestVoxels8_Dilate(int size, int iterations) {
    Voxels8 a(size, size, size);
    Timer timer;

    a.set(1, 1, 1, 1);

    for (int i=0; i< iterations; i++) {
        Voxels8 *b = a.dilate(1);
        delete b;
    }
    return timer.elapsed();
}


double TestVoxels8_GetBoundingRangeAndCount(int size, int iterations) {
    Voxels8 a(size, size, size);
    Timer timer;

    a.set(1, 1, 1, 1);

    for (int i=0; i< iterations; i++) {
        a.getBoundingRangeAndCount();
    }
    return timer.elapsed();
}


double TestVoxels8_IsEqual(int size, int iterations) {
    Voxels8 a(size, size, size);
    Voxels8 b(size, size, size);
    Timer timer;

    for (int i=0; i< iterations; i++) {
        a.isEqual(b);
    }
    return timer.elapsed();
}


double TestVoxelsPacked_Subtract(int size, int iterations) {
    VoxelsPacked a(size, size, size);
    VoxelsPacked b(size, size, size);
    Timer timer;

    for (int i=0; i< iterations; i++) {
        a.subtract(b);
    }
    return timer.elapsed();
}

double TestVoxelsPacked_IsEqual(int size, int iterations) {
    VoxelsPacked a(size, size, size);
    VoxelsPacked b(size, size, size);
    Timer timer;

    for (int i=0; i< iterations; i++) {
        a.isEqual(b);
    }
    return timer.elapsed();
}

double TestVoxelsPacked_Dilate(int size, int iterations) {
    VoxelsPacked a(size, size, size);
    Timer timer;

    a.set(1, 1, 1, 1);

    for (int i=0; i< iterations; i++) {
        VoxelsPacked *b = a.dilate(1);
        delete b;
    }
    return timer.elapsed();
}


double TestVoxelsPacked_GetBoundingRangeAndCount(int size, int iterations) {
    VoxelsPacked a(size, size, size);
    Timer timer;

    a.set(1, 1, 2, 1);
    a.set(4, 2, 1, 1);

    for (int i=0; i< iterations; i++) {
        a.getBoundingRangeAndCount();
    }
    return timer.elapsed();
}


void run_test(const std::string message, double (*func1)(int, int), double (*func2)(int, int), int size, int iterations) {
    double voxels_8 = func1(size, iterations);
    double voxels_long = func2(size, iterations);

    std::cout << "\n" << message << std::endl;
    std::cout << "TestVoxels8: " << voxels_8 << std::endl;
    std::cout << "TestVoxelsPacked: " << voxels_long << std::endl;
    std::cout << "Speedup: " << voxels_8 / voxels_long << std::endl;
}

int main() {
    int size = 64;
    int iterations = 1;


    std::cout << "COMPARE" << std::endl;
    {
        Voxels8 voxels_8(size, size, size);
        VoxelsPacked voxels_packed(size, size, size);

        voxels_8.set(1, 1, 1, 1);
        voxels_packed.set(1, 1, 1, 1);
        std::cout << "TestVoxels8: " << voxels_8.getCount() << std::endl;
        std::cout << "TestVoxelsPacked: " << voxels_packed.getCount() << std::endl;
        std::cout << "Voxels8 bytes: " << voxels_8.bytes() << std::endl;
        std::cout << "VoxelsPacked bytes: " << voxels_packed.bytes() << std::endl;
        std::cout << "TestVoxelsPacked bits per word: " << voxels_packed.bitsPerWord() << std::endl;

    }

//    run_test("SUBTRACT", TestVoxels8_Subtract, TestVoxelsPacked_Subtract, size, iterations);
//    run_test("DILATE", TestVoxels8_Dilate, TestVoxelsPacked_Dilate, size, iterations);
//    run_test("ISEQUAL", TestVoxels8_IsEqual, TestVoxelsPacked_IsEqual, size, iterations);
    run_test("GETBOUNDINGRANGEANDCOUNT", TestVoxels8_GetBoundingRangeAndCount, TestVoxelsPacked_GetBoundingRangeAndCount, size, iterations);

    return 0;
}

#if 0
16.02      2.99     2.99      377     0.01     0.01  RawVoxelVolume::subtract(RawVoxelVolume const&)
 12.27      5.28     2.29       60     0.04     0.04  RawVoxelVolume::isEqual(RawVoxelVolume*)
 11.84      7.49     2.21      117     0.02     0.02  RawVoxelVolume::dilate(unsigned char) const
  7.66      8.92     1.43      249     0.01     0.01  RawVoxelVolume::getBoundingRangeAndCount()
  7.07     10.24     1.32       97     0.01     0.01  NewKernelTypes::VoxelBoundaryExtractor::ExtractBoundary()
  4.93     11.16     0.92      163     0.01     0.01  RawVoxelVolume::setUnion(RawVoxelVolume*)
  4.72     12.04     0.88        1     0.88     0.89  RawVoxelVolume::getBoundingBox()
  4.56     12.89     0.85       38     0.02     0.04  VoxelVolume::getCenterOfMass(double*, bool) const
  4.29     13.69     0.80      222     0.00     0.00  RawVoxelVolume::intersect(VoxelVolume*)
  3.97     14.43     0.74       83     0.01     0.01  RawVoxelVolume::diagonally_dilate(unsigned char) const
  3.80     15.14     0.71      178     0.00     0.00  RawVoxelVolume::countVoxels()
  3.16     15.73     0.59 297959102     0.00     0.00  RawVoxelVolume::operator()(unsigned int) const
  1.29     17.20     0.24       19     0.01     0.01  RawVoxelVolume::RotatedVoxelGridAND(RawVoxelVolume*, unsigned int, unsigned int, unsigned int, float, float, RawVoxelVolume*)
#endif
