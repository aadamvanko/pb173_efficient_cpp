#include <iostream>
#include <cassert>
#include <array>
#include <set>
#include <vector>
#include <random>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <algorithm>

#define BENCHMARKING_RUN(function, data) Benchmarking::run(#function, function, data)

namespace Benchmarking
{
    namespace
    {
        enum class Profiles { TIME, PRECISION } profile = Profiles::TIME;
        int timeLimit = 5; // seconds
        int precisionLimit = 2; // percent

        const int BENCHMARK_COUNT_LIMIT = 10000; // due to very short benchmarks
        const int PRECISION_INITIAL_TIME_LIMIT = 5; // seconds
        const int PRECISION_STEP_ADDITIONAL_BENCHMARKS = 10;
        const int BOOTSTRAP_CYCLES_COUNT = 10000;
        const long long NANOSECONDS_IN_SECOND = 1000000000;

        timespec startTime;
        timespec stopTime;

        std::vector<long long> measuredTimes;
        std::string sizeInfo;

        void run_benchmark_once(void (*functionToBenchmark)(void*), void* data)
        {
            functionToBenchmark(data);
            long long measuredTime = (stopTime.tv_sec - startTime.tv_sec) * NANOSECONDS_IN_SECOND +
                                             (stopTime.tv_nsec - startTime.tv_nsec);
            measuredTimes.push_back(measuredTime);
        }

        void run_benchmark_for_time(int limit, void (*functionToBenchmark)(void*), void* data)
        {
            timespec benchmarkStartTime, benchmarkEndTime;
            clock_gettime(CLOCK_MONOTONIC, &benchmarkStartTime);
            clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
            int benchmarkCount = 0;
            functionToBenchmark(data); // cold start
            while (benchmarkEndTime.tv_sec - benchmarkStartTime.tv_sec < limit && benchmarkCount < BENCHMARK_COUNT_LIMIT)
            {
                run_benchmark_once(functionToBenchmark, data);
                clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
                benchmarkCount++;
            }
        }

        struct BootstrappingResults
        {
            double mCILow;
            double aCILow;
            double mCIHigh;
            double aCIHigh;
            double bootstrappedValuesAverage;
        };

        BootstrappingResults calculate_bootstrap_stats()
        {
            std::random_device randomDevice;
            std::mt19937 generator(randomDevice());
            std::uniform_int_distribution<> distribution(0, measuredTimes.size() - 1);

            std::vector<double> bootstrappedValues;
            for (int i = 0; i < BOOTSTRAP_CYCLES_COUNT; i++)
            {
                std::vector<long long> selectedTimes;
                long long suma = 0;
                for (unsigned j = 0; j < measuredTimes.size(); j++)
                {
                    unsigned index = distribution(generator);
                    selectedTimes.push_back(measuredTimes[index]);
                    suma += measuredTimes[index];
                }
                long long sum = std::accumulate(selectedTimes.begin(), selectedTimes.end(), 0LL);
                double average = sum / (double)NANOSECONDS_IN_SECOND / (double)selectedTimes.size(); // estimator
                bootstrappedValues.push_back(average);
            }

            double bootstrappedValuesSum = std::accumulate(bootstrappedValues.begin(), bootstrappedValues.end(), 0.0);
            double bootstrappedValuesAverage = bootstrappedValuesSum / bootstrappedValues.size();

            // for percentile finding
            std::sort(bootstrappedValues.begin(), bootstrappedValues.end());

            // calculate standard deviation
            double sd = std::sqrt(std::accumulate(bootstrappedValues.begin(), bootstrappedValues.end(), 0.0,
                                        [bootstrappedValuesAverage](double computedSd, double bootstrappedValue)
            {
                return computedSd + (bootstrappedValue - bootstrappedValuesAverage) * (bootstrappedValue - bootstrappedValuesAverage);
            }) / bootstrappedValues.size());

            double mCILow = bootstrappedValues[unsigned(bootstrappedValues.size() * 0.05)];
            double aCILow = bootstrappedValuesAverage - 2 * sd;
            double mCIHigh = bootstrappedValues[unsigned(bootstrappedValues.size() * 0.95)];
            double aCIHigh = bootstrappedValuesAverage + 2 * sd;
            return { mCILow, aCILow, mCIHigh, aCIHigh, bootstrappedValuesAverage };
        }
    }

    void init(int argc, char* argv[])
    {
        if (argc != 3)
        {
            std::cout << "Wrong number of arguments given to program!" << std::endl;
            std::cout << "Using default settings, time profile with limit " << timeLimit << " seconds and maximal number of benchmarks " << BENCHMARK_COUNT_LIMIT << "." << std::endl;
            return;
        }

        if (std::strcmp(argv[1], "time") == 0)
        {
            timeLimit = std::stoi(argv[2]);
            profile = Profiles::TIME;
        }
        else if (std::strcmp(argv[1], "prec") == 0)
        {
            precisionLimit = std::stoi(argv[2]);
            profile = Profiles::PRECISION;
        }
        else
        {
            std::cout << "Wrong parameter " << argv[1] << ", expected time or prec!" << std::endl;
        }
    }

    void size_info(const std::string& str)
    {
        sizeInfo = str;
    }

    void run(const char* name, void (*functionToBenchmark)(void* data), void* data)
    {
        sizeInfo = "unknown size";
        BootstrappingResults bootstrapResults;

        if (profile == Profiles::TIME)
        {
            // std::cout << "Running time benchmark..." << std::endl;
            run_benchmark_for_time(timeLimit, functionToBenchmark, data);

            // std::cout << "Bootstrapping..." << std::endl;
            bootstrapResults = calculate_bootstrap_stats();
        }
        else
        {
            // std::cout << "Running precision benchmark..." << std::endl;
            run_benchmark_for_time(PRECISION_INITIAL_TIME_LIMIT, functionToBenchmark, data);

            bootstrapResults = calculate_bootstrap_stats();
            // std::cout << "CI wanted diff = " << precisionLimit / 100.0 << std::endl;
            // std::cout << "CI width = " << bootstrapResults.mCIHigh / bootstrapResults.mCILow - 1 << std::endl;
            while (bootstrapResults.mCIHigh / bootstrapResults.mCILow >= (1.0 + precisionLimit / 100.0))
            {
                // std::cout << "Not enough precision, running one more benchmark..." << std::endl;
                for (int i = 0; i < PRECISION_STEP_ADDITIONAL_BENCHMARKS; i++)
                {
                    run_benchmark_once(functionToBenchmark, data);
                }
                bootstrapResults = calculate_bootstrap_stats();
                // std::cout << "CI width = " << bootstrapResults.mCIHigh / bootstrapResults.mCILow - 1<< std::endl;
            }
        }

        std::cout << name << ", "
                  << sizeInfo << ", "
                  << static_cast<long long>(bootstrapResults.mCILow * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.aCILow * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.bootstrappedValuesAverage * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.mCIHigh * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.aCIHigh * NANOSECONDS_IN_SECOND) << std::endl;
        measuredTimes.clear();
    }

    void start()
    {
        clock_gettime(CLOCK_MONOTONIC, &startTime);
    }

    void stop()
    {
        clock_gettime(CLOCK_MONOTONIC, &stopTime);
    }
}
