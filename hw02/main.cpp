#include <iostream>
#include <random>

#include "../hw01/benchmark_tool.hpp"
#include "matrix2d.hpp"

struct BenchmarkData
{
    Matrix2D matrixA;
    Matrix2D matrixB;
    int blockSize;
};

Matrix2D generateRandomMatrix(std::mt19937& generator, int size)
{
    Matrix2D matrix(size, size);
    std::uniform_int_distribution<> dist(0, 1);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = dist(generator);
        }
    }

    return matrix;
}

void naive(void* data)
{
    const BenchmarkData& matrices = *(BenchmarkData*)data;

    Benchmarking::size_info("size=" + std::to_string(matrices.matrixA.size()));
    Benchmarking::start();
    Matrix2D result = matrices.matrixA.naiveMultiplication(matrices.matrixB);
    Benchmarking::stop();
    auto s = result.size();
    s ^= s;
}

void cache_eff(void* data)
{
    const BenchmarkData& matrices = *(BenchmarkData*)data;

    Benchmarking::size_info("size=" + std::to_string(matrices.matrixA.size()) +
                            ", block_size=" + std::to_string(matrices.blockSize));
    Benchmarking::start();
    Matrix2D result = matrices.matrixA.cacheEfficientMultiplication(matrices.matrixB, matrices.blockSize);
    Benchmarking::stop();
    auto s = result.size();
    s ^= s;
}

int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);

    std::mt19937 generator;
    generator.seed(1);
    std::uniform_int_distribution<> dist(0, 1);

    BenchmarkData benchmarkData;
    for (int size = 2; size <= 1024; size *= 2)
    {
        benchmarkData.matrixA = generateRandomMatrix(generator, size);
        benchmarkData.matrixB = generateRandomMatrix(generator, size);

        auto correctResult = benchmarkData.matrixA.naiveMultiplication(benchmarkData.matrixB);
        BENCHMARKING_RUN(naive, &benchmarkData);

        for (int blockSize = 2; blockSize <= 64 && size % blockSize == 0; blockSize *= 2)
        {
            benchmarkData.blockSize = blockSize;
            auto efficientResult = benchmarkData.matrixA.cacheEfficientMultiplication(benchmarkData.matrixB, blockSize);
            // cachedResult.print(std::cout);
            if (correctResult != efficientResult)
            {
                std::cout << "ERROR IN MULTIPLICATION OF cacheEfficient!" << std::endl;
            }
            BENCHMARKING_RUN(cache_eff, &benchmarkData);
        }
    }

    return 0;
}
