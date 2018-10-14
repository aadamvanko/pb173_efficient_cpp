#include <iostream>
#include <random>

#include "../hw01/benchmark_tool.hpp"
#include "matrix2d.hpp"

struct BenchmarkData
{
    Matrix2D matrixA;
    Matrix2D matrixB;
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

    Benchmarking::size_info("size=" + std::to_string(matrices.matrixA.size()));
    Benchmarking::start();
    Matrix2D result = matrices.matrixA.cacheEfficientMultiplication(matrices.matrixB);
    Benchmarking::stop();
    auto s = result.size();
    s ^= s;
}

void transposed(void* data)
{
    const BenchmarkData& matrices = *(BenchmarkData*)data;

    Benchmarking::size_info("size=" + std::to_string(matrices.matrixA.size()));
    Benchmarking::start();
    Matrix2D result = matrices.matrixA.transposedMultiplication(matrices.matrixB);
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
    for (int size = 4; size <= 128; size *= 2)
    {
        benchmarkData.matrixA = generateRandomMatrix(generator, size);
        benchmarkData.matrixB = generateRandomMatrix(generator, size);

        benchmarkData.matrixA.print(std::cout);
        std::cout << std::endl;
        benchmarkData.matrixB.print(std::cout);
        std::cout << std::endl;

        Matrix2D correctResult = benchmarkData.matrixA.naiveMultiplication(benchmarkData.matrixB);
        //correctResult.print(std::cout);

        auto cachedResult = benchmarkData.matrixA.cacheEfficientMultiplication(benchmarkData.matrixB);
        cachedResult.print(std::cout);
        if (correctResult != benchmarkData.matrixA.cacheEfficientMultiplication(benchmarkData.matrixB))
        {
            std::cout << "ERROR IN MULTIPLICATION OF cacheEfficient!" << std::endl;
        }
        if (correctResult != benchmarkData.matrixA.transposedMultiplication(benchmarkData.matrixB))
        {
            std::cout << "ERROR IN MULTIPLICATION OF transposed!" << std::endl;
        }

        BENCHMARKING_RUN(naive, &benchmarkData);
        BENCHMARKING_RUN(cache_eff, &benchmarkData);
        BENCHMARKING_RUN(transposed, &benchmarkData);
    }

    return 0;
}
