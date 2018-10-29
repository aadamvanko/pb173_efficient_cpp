#include <iostream>
#include <string>
#include "../hw01/benchmark_tool.hpp"

void indirectForeach(void (*)(void));

void functDo()
{
    volatile int sum = 0;
    sum++;
    sum ^= sum;
}

void benchmark_direct_call(void* data)
{
    const int callsCount = *(int*)data;
    Benchmarking::size_info("calls=" + std::to_string(callsCount));
    Benchmarking::start();
    for (int i = 0; i < callsCount; i++)
    {
        functDo();
    }
    Benchmarking::stop();
}

void benchmark_indirect_call(void* data)
{
    const int callsCount = *(int*)data;
    Benchmarking::size_info("calls=" + std::to_string(callsCount));
    Benchmarking::start();
    for (int i = 0; i < callsCount; i++)
    {
        indirectForeach(functDo);
    }
    Benchmarking::stop();
}

int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);
    int CALLS_COUNT = 100;

    BENCHMARKING_RUN(benchmark_direct_call, &CALLS_COUNT);
    BENCHMARKING_RUN(benchmark_indirect_call, &CALLS_COUNT);

    return 0;
}
