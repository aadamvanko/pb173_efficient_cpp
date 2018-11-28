#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <vector>
#include <atomic>
#include <thread>

#include "../hw01/benchmark_tool.hpp"

using namespace std;

struct BenchmarkData {
    const int blockSize;
    const vector<int>& numbers;
};

int correctResult = 0;
const int NUMBERS_COUNT = 1073741824;

vector<int> prepareNumbers() {
    vector<int> numbers;
    random_device rd;
    uniform_int_distribution<int> dist(0, 10);
    for (int i = 0; i < NUMBERS_COUNT; i++) {
        numbers.push_back(dist(rd));
    }
    return numbers;
}

volatile long long benchmarkReduceResult;

void benchmarkNaiveIndexing(void* data) {
    const BenchmarkData& benchmarkData = *(BenchmarkData*)data;
    Benchmarking::size_info("numbersCount=" + to_string(benchmarkData.numbers.size()));
    Benchmarking::start();
    long long result = 0;
    for (int i = 0; i < benchmarkData.numbers.size(); i++) {
        result += benchmarkData.numbers[i];
    }
    benchmarkReduceResult = result;
    Benchmarking::stop();
}

void benchmarkNaiveIterator(void* data) {
    const BenchmarkData& benchmarkData = *(BenchmarkData*)data;
    Benchmarking::size_info("numbersCount=" + to_string(benchmarkData.numbers.size()));
    Benchmarking::start();
    long long result = 0;
    for (auto it = benchmarkData.numbers.begin(); it != benchmarkData.numbers.end(); it++) {
        result += *it;
    }
    benchmarkReduceResult = result;
    Benchmarking::stop();
}

void benchmarkMapReduce(void* data) {
    const BenchmarkData& benchmarkData = *(BenchmarkData*)data;
    Benchmarking::size_info("numbersCount=" + to_string(benchmarkData.numbers.size()) + ", blockSize=" + to_string(benchmarkData.blockSize));
    Benchmarking::start();
    atomic<long long> result;
    const int threadsCount = 2;
    thread threads[threadsCount];
    for (int i = 0; i < threadsCount; i++) {
        int from = i * benchmarkData.blockSize;
        int to = (i + 1) * benchmarkData.blockSize;
        threads[i] = thread([&](const int from, const int to) {
            long long localResult = 0;
            for (int i = from; i < to; i++) {
                localResult += benchmarkData.numbers[i];
            }
            result += localResult;
        }, from, to);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    benchmarkReduceResult = result;
    Benchmarking::stop();
}

int main(int argc, char** argv) {
    Benchmarking::init(argc, argv);

    const std::vector<int> numbers = prepareNumbers();
    correctResult = std::accumulate(numbers.begin(), numbers.end(), 0LL);

    BENCHMARKING_RUN(benchmarkNaiveIndexing, nullptr);
    assert(correctResult == benchmarkReduceResult);
    BENCHMARKING_RUN(benchmarkNaiveIterator, nullptr);
    assert(correctResult == benchmarkReduceResult);
    for (int i = 0; i <= 29; i++) {
        int blockSize = 1 << i;
        if (numbers.size() % blockSize != 0) {
            continue;
        }
        BenchmarkData data { blockSize, numbers };
        BENCHMARKING_RUN(benchmarkMapReduce, &data);
        assert(correctResult == benchmarkReduceResult);
    }

    return 0;
}
