#include <iostream>
#include <list>
#include <vector>

#include "benchmark_tool.hpp"

using std::list;
using std::vector;

const int ACCESSED_ITEMS_COUNT = 5;

void benchmark_vector_access_random_sequence(void* data)
{
    std::pair<int, int>& settings = *(std::pair<int, int>*)data;
    const int size = settings.first;
    const int randomPosition = settings.second;

    int sum = 0;
    vector<int> v(size, 0);
    Benchmarking::size_info(std::to_string(size));
    Benchmarking::start();
    for (int i = randomPosition; i < randomPosition + ACCESSED_ITEMS_COUNT && i < v.size(); i++) {
        sum += v[i];
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_list_access_random_sequence(void* data)
{
    std::pair<int, int>& settings = *(std::pair<int, int>*)data;
    const int size = settings.first;
    const int randomPosition = settings.second;

    int sum = 0;
    std::list<int> list(size, 0);
    const auto sequenceEnd = std::next(list.begin(), randomPosition + ACCESSED_ITEMS_COUNT);
    Benchmarking::size_info(std::to_string(size));
    Benchmarking::start();
    for (auto it = std::next(list.begin(), randomPosition); it != list.end() && it != sequenceEnd; it++) {
        sum += *it;
    }
    Benchmarking::stop();
    sum ^= sum;
}

int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);
    for (int vectorSize = 10; vectorSize <= 1000000; vectorSize *= 10) {
        std::pair<int, int> settings{vectorSize, rand() % vectorSize};
        BENCHMARKING_RUN(benchmark_vector_access_random_sequence, &settings);
        BENCHMARKING_RUN(benchmark_list_access_random_sequence, &settings);
    }

    return 0;
}
