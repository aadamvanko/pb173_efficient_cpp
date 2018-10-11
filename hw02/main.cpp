#include <iostream>

#include "../hw01/benchmark_tool.hpp"
#include "matrix2d.hpp"

void naive(void* data)
{
    const int size = *(int*)data;
    Matrix2D matrixA(size, size);
    Matrix2D matrixB(size, size);
    Benchmarking::size_info("size=" + std::to_string(size));
    Benchmarking::start();
    Matrix2D result = matrixA.naiveMultiplication(matrixB);
    Benchmarking::stop();
    auto s = result.size();
    s ^= s;
}

void cache_eff(void* data)
{
    const int size = *(int*)data;
    Matrix2D matrixA(size, size);
    Matrix2D matrixB(size, size);
    Benchmarking::size_info("size=" + std::to_string(size));
    Benchmarking::start();
    Matrix2D result = matrixA.cacheEfficientMultiplication(matrixB);
    Benchmarking::stop();
    auto s = result.size();
    s ^= s;
}

int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);
    for (int size = 10; size <= 1000; size *= 10)
    {
        BENCHMARKING_RUN(naive, &size);
        BENCHMARKING_RUN(cache_eff, &size);
    }
    return 0;
}
