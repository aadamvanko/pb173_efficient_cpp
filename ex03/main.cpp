#include <iostream>
#include <list>
#include <vector>

#include "benchmark_tool.hpp"

using std::list;
using std::vector;

const int ACCESSED_ITEMS_COUNT = 5;

void benchmark_vector_10_access_random_sequence(void* data)
{
    int sum = 0;
    vector<int> v(10, 2);
    Benchmarking::start();
    for (int i = v.size() / 2; i < v.size(); i++) {
        sum += v[i];
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_vector_100_access_random_sequence(void* data)
{
    int sum = 0;
    vector<int> v(100, 2);
    Benchmarking::start();
    for (int i = v.size() / 2; i < v.size(); i++) {
        sum += v[i];
    }
    Benchmarking::stop();
    sum ^= sum;
}

void benchmark_vector_10000_access_random_sequence(void* data)
{
    int sum = 0;
    vector<int> v(10000, 2);
    Benchmarking::start();
    for (int i = v.size() / 2; i < v.size(); i++) {
        sum += v[i];
    }
    Benchmarking::stop();
    sum ^= sum;
}


int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);
    //BENCHMARKING_RUN(benchmark_vector_10_access_random_sequence, NULL);

    return 0;
}
