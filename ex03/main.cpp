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
    Benchmarking::size_info("vector_size=" + std::to_string(v.size()) + ", sequence_start=" + std::to_string(randomPosition) + ", sequence_length=" + std::to_string(ACCESSED_ITEMS_COUNT));
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
    Benchmarking::size_info("list_size=" + std::to_string(list.size()) + ", sequence_start=" + std::to_string(randomPosition) + ", sequence_length=" + std::to_string(ACCESSED_ITEMS_COUNT));
    Benchmarking::start();
    for (auto it = std::next(list.begin(), randomPosition); it != list.end() && it != sequenceEnd; it++) {
        sum += *it;
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_vector_access_random_elements(void* data)
{
    int size = *(int*)data;
    int sum = 0;
    vector<int> v(size, 0);

    const int ACCESSES_COUNT = 10;
    std::vector<int> positions;
    for (int i = 0; i < ACCESSES_COUNT; i++) {
        positions.push_back(rand() % v.size());
    }

    Benchmarking::size_info("vector_size=" + std::to_string(v.size()) + ", accessed_positions=" + std::to_string(positions.size()));
    Benchmarking::start();
    for (int position : positions) {
        sum += v[position];
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_vector_for_loop(void* data)
{
    int size = *(int*)data;
    int sum = 0;
    vector<int> v(size, 0);

    Benchmarking::size_info("vector_size=" + std::to_string(v.size()));
    Benchmarking::start();
    for (unsigned i = 0; i < v.size(); i++) {
        sum += v[i];
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_vector_for_each(void* data)
{
    int size = *(int*)data;
    int sum = 0;
    vector<int> v(size, 0);

    Benchmarking::size_info("vector_size=" + std::to_string(v.size()));
    Benchmarking::start();
    for (int number : v) {
        sum += number;
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_vector_iterator(void* data)
{
    int size = *(int*)data;
    int sum = 0;
    vector<int> v(size, 0);

    Benchmarking::size_info("vector_size=" + std::to_string(v.size()));
    Benchmarking::start();
    for (auto it = v.begin(); it != v.end(); it++) {
        sum += *it;
    }
    Benchmarking::stop();
    sum ^= sum;
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    Benchmarking::init(argc, argv);
    for (int vectorSize = 10; vectorSize <= 1000000; vectorSize *= 10) {
        std::pair<int, int> settings{vectorSize, rand() % vectorSize};
        BENCHMARKING_RUN(benchmark_vector_access_random_sequence, &settings);
        BENCHMARKING_RUN(benchmark_list_access_random_sequence, &settings);
        BENCHMARKING_RUN(benchmark_vector_access_random_elements, &vectorSize);
    }

    for (int vectorSize = 10; vectorSize <= 1000000; vectorSize *= 10) {
        BENCHMARKING_RUN(benchmark_vector_for_loop, &vectorSize);
        BENCHMARKING_RUN(benchmark_vector_for_each, &vectorSize);
        BENCHMARKING_RUN(benchmark_vector_iterator, &vectorSize);
    }

    return 0;
}
