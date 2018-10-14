#include "benchmark_tool.hpp"
#include "ex01.hpp"

void benchmark_priority_buffer_array_5(void* data)
{
    std::vector<int>& randomNumbers = *(std::vector<int>*)data;
    PriorityBufferArray<5> pba5;
    Benchmarking::size_info("container_capacity=" + std::to_string(5) + ", inserts_count=" + std::to_string(randomNumbers.size()));
    Benchmarking::start();
    for (int x : randomNumbers)
        pba5.insert(x);
    Benchmarking::stop();
}

void benchmark_priority_buffer_array_10(void* data)
{
    std::vector<int>& randomNumbers = *(std::vector<int>*)data;
    PriorityBufferArray<10> pba10;
    Benchmarking::size_info("container_capacity=" + std::to_string(10) + ", inserts_count=" + std::to_string(randomNumbers.size()));
    Benchmarking::start();
    for (int x : randomNumbers)
        pba10.insert(x);
    Benchmarking::stop();
}

void benchmark_priority_buffer_array_10000(void* data)
{
    std::vector<int>& randomNumbers = *(std::vector<int>*)data;
    PriorityBufferArray<10000> pba10000;
    Benchmarking::size_info("container_capacity=" + std::to_string(10000) + ", inserts_count=" + std::to_string(randomNumbers.size()));
    Benchmarking::start();
    for (int x : randomNumbers)
        pba10000.insert(x);
    Benchmarking::stop();
}

void benchmark_priority_buffer_set_5(void* data)
{
    std::vector<int>& randomNumbers = *(std::vector<int>*)data;
    PriorityBufferSet<5> pbs5;
    Benchmarking::size_info("container_capacity=" + std::to_string(5) + ", inserts_count=" + std::to_string(randomNumbers.size()));
    Benchmarking::start();
    for (int x : randomNumbers)
        pbs5.insert(x);
    Benchmarking::stop();
}

void benchmark_priority_buffer_set_10(void* data)
{
    std::vector<int>& randomNumbers = *(std::vector<int>*)data;
    PriorityBufferSet<10> pbs10;
    Benchmarking::size_info("container_capacity=" + std::to_string(10) + ", inserts_count=" + std::to_string(randomNumbers.size()));
    Benchmarking::start();
    for (int x : randomNumbers)
        pbs10.insert(x);
    Benchmarking::stop();
}

void benchmark_priority_buffer_set_10000(void* data)
{
    std::vector<int>& randomNumbers = *(std::vector<int>*)data;
    PriorityBufferSet<10000> pbs10000;
    Benchmarking::size_info("container_capacity=" + std::to_string(10000) + ", inserts_count=" + std::to_string(randomNumbers.size()));
    Benchmarking::start();
    for (int x : randomNumbers)
        pbs10000.insert(x);
    Benchmarking::stop();
}

int main(int argc, char* argv[])
{
    Benchmarking::init(argc, argv);

    // prepare random numbers for benchmarking
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 1'000'000'000);

    const int randomNumbersCnt = 7'000'000;
    std::vector<int> randomNumbers(randomNumbersCnt);
    for (int i = 0; i < randomNumbersCnt; i++)
    {
        randomNumbers[i] = dist(rng);
    }

    Benchmarking::run("benchmark_priority_buffer_array", benchmark_priority_buffer_array_5, &randomNumbers);
    Benchmarking::run("benchmark_priority_buffer_array", benchmark_priority_buffer_array_10, &randomNumbers);
    Benchmarking::run("benchmark_priority_buffer_array", benchmark_priority_buffer_array_10000, &randomNumbers);

    Benchmarking::run("benchmark_priority_buffer_set", benchmark_priority_buffer_set_5, &randomNumbers);
    Benchmarking::run("benchmark_priority_buffer_set", benchmark_priority_buffer_set_10, &randomNumbers);
    Benchmarking::run("benchmark_priority_buffer_set", benchmark_priority_buffer_set_10000, &randomNumbers);

    return 0;
}
