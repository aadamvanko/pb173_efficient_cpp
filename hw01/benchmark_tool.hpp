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
        int timeModeTimeLimit = 5; // seconds
        int precisionModePrecisionLimit = 20; // percent

        const int TIME_BENCHMARK_COUNT_LIMIT = 10000; // due to very short benchmarks
        const int PRECISION_INITIAL_TIME_LIMIT = 5; // seconds
        const int PRECISION_STEP_ADDITIONAL_TIME_LIMIT = 3; // seconds
        const int PRECISION_STEP_ADDITIONAL_BENCHMARK_LIMIT = 5000; // due to very short benchmarks
        const int PRECISION_ADDITIONL_STEPS_COUNT = 10; // steps/cycles
        const int BOOTSTRAP_CYCLES_COUNT = 10000;
        const long long NANOSECONDS_IN_SECOND = 1000000000;

        timespec startTime;
        timespec stopTime;

        std::vector<long long> measuredTimes;
        std::string sizeInfo;

        void idle()
        {
            volatile int idleCounter = 0;
            for (int i = 0; i < 1000000000; i++)
            {
                idleCounter++;
            }
            idleCounter ^= idleCounter;
        }

        void run_benchmark_for_time(const int timeLimit, void (*functionToBenchmark)(void*), void* data, const int benchmarkCountLimit)
        {
            idle(); // anti frequency scaling

            timespec benchmarkStartTime, benchmarkEndTime;
            clock_gettime(CLOCK_MONOTONIC, &benchmarkStartTime);
            clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
            int benchmarkCount = 0;
            functionToBenchmark(data); // cold start
            while (benchmarkEndTime.tv_sec - benchmarkStartTime.tv_sec < timeLimit && benchmarkCount < benchmarkCountLimit)
            {
                functionToBenchmark(data);
                long long measuredTime = (stopTime.tv_sec - startTime.tv_sec) * NANOSECONDS_IN_SECOND +
                                                 (stopTime.tv_nsec - startTime.tv_nsec);
                measuredTimes.push_back(measuredTime);
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
            std::mt19937 generator;
            generator.seed(std::random_device()());
            std::uniform_int_distribution<> distribution(0, measuredTimes.size() - 1);

            std::vector<double> bootstrappedValues;
            std::vector<long long> bootstrappedLowPercentiles;
            std::vector<long long> bootstrappedHighPercentiles;
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

                std::sort(selectedTimes.begin(), selectedTimes.end());
                bootstrappedLowPercentiles.push_back(selectedTimes[static_cast<unsigned>(0.05 * selectedTimes.size())]);
                bootstrappedHighPercentiles.push_back(selectedTimes[static_cast<unsigned>(0.95 * selectedTimes.size())]);
            }

            double bootstrappedValuesSum = std::accumulate(bootstrappedValues.begin(), bootstrappedValues.end(), 0.0);
            double bootstrappedValuesAverage = bootstrappedValuesSum / bootstrappedValues.size();

            long long bootstrappedLowPercentilesSum = std::accumulate(bootstrappedLowPercentiles.begin(),
                                                                       bootstrappedLowPercentiles.end(), 0LL);
            long long bootstrappedHighPercentilesSum = std::accumulate(bootstrappedHighPercentiles.begin(),
                                                                       bootstrappedHighPercentiles.end(), 0LL);

            // calculate standard deviation
            double sumSquaredDifferences = 0.0;
            for (const double value : bootstrappedValues)
            {
                sumSquaredDifferences += std::pow(value - bootstrappedValuesAverage, 2);
            }
            double sd = std::sqrt(sumSquaredDifferences / bootstrappedValues.size());

            double mCILow = bootstrappedLowPercentilesSum / static_cast<double>(bootstrappedLowPercentiles.size());
            double aCILow = bootstrappedValuesAverage - 2 * sd;
            double mCIHigh = bootstrappedHighPercentilesSum / static_cast<double>(bootstrappedHighPercentiles.size());
            double aCIHigh = bootstrappedValuesAverage + 2 * sd;
            return { mCILow / static_cast<double>(NANOSECONDS_IN_SECOND),
                        aCILow,
                        mCIHigh / static_cast<double>(NANOSECONDS_IN_SECOND),
                        aCIHigh,
                        bootstrappedValuesAverage };
        }
    }

    void init(int argc, char* argv[])
    {
        if (argc != 3)
        {
            std::cout << "Wrong number of arguments given to program!" << std::endl;
            std::cout << "Using default settings, time profile with limit " << timeModeTimeLimit << " seconds and maximal number of benchmarks " << TIME_BENCHMARK_COUNT_LIMIT << "." << std::endl;
            return;
        }

        if (std::strcmp(argv[1], "time") == 0)
        {
            timeModeTimeLimit = std::stoi(argv[2]);
            profile = Profiles::TIME;
        }
        else if (std::strcmp(argv[1], "prec") == 0)
        {
            precisionModePrecisionLimit = std::stoi(argv[2]);
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

    void run(const std::string& benchmark_name, void (*functionToBenchmark)(void* data), void* data)
    {
        sizeInfo = "unknown size";
        BootstrappingResults bootstrapResults;
        std::cout << std::fixed << std::setprecision(10);

        if (profile == Profiles::TIME)
        {
            // std::cout << "Running time benchmark..." << std::endl;
            run_benchmark_for_time(timeModeTimeLimit, functionToBenchmark, data, TIME_BENCHMARK_COUNT_LIMIT);

            // std::cout << "Bootstrapping..." << std::endl;
            bootstrapResults = calculate_bootstrap_stats();
        }
        else
        {
            //std::cout << "Running precision benchmark..." << std::endl;
            run_benchmark_for_time(PRECISION_INITIAL_TIME_LIMIT, functionToBenchmark, data, TIME_BENCHMARK_COUNT_LIMIT);

            bootstrapResults = calculate_bootstrap_stats();
/*
            std::cout << "CI wanted diff = " << precisionModePrecisionLimit / 100.0 << ", "
                        << "CI width = " << bootstrapResults.mCIHigh / bootstrapResults.mCILow - 1 << ", "
                        << "mCILow = " << bootstrapResults.mCILow << ", "
                        << "mCIHigh = " << bootstrapResults.mCIHigh << std::endl;
*/
            double mCILowDiff = bootstrapResults.bootstrappedValuesAverage - bootstrapResults.mCILow;
            double mCIHighDiff = bootstrapResults.mCIHigh - bootstrapResults.bootstrappedValuesAverage;
            double CIWidthLimit = bootstrapResults.bootstrappedValuesAverage * precisionModePrecisionLimit / 100;

            if (mCILowDiff > CIWidthLimit || mCIHighDiff > CIWidthLimit)
            {
                int i = 0;
                for (; i < PRECISION_ADDITIONL_STEPS_COUNT; i++)
                {
                    // std::cout << "Not enough precision, running one more benchmark..." << std::endl;
                    run_benchmark_for_time(PRECISION_STEP_ADDITIONAL_TIME_LIMIT, functionToBenchmark, data, PRECISION_STEP_ADDITIONAL_BENCHMARK_LIMIT);
                    bootstrapResults = calculate_bootstrap_stats();
/*
                    std::cout << "CI width = " << bootstrapResults.mCIHigh / bootstrapResults.mCILow - 1 << ", "
                              << "mCILow = " << bootstrapResults.mCILow << ", "
                              << "mCIHigh = " << bootstrapResults.mCIHigh << std::endl;
*/
                    if (mCILowDiff > CIWidthLimit || mCIHighDiff > CIWidthLimit)
                    {
                        // good enough precision
                        break;
                    }
                }

                if (i == PRECISION_ADDITIONL_STEPS_COUNT)
                {
                    std::cout << "TIMED OUT, ";
                }
            }
        }

        std::cout << benchmark_name << ", "
                  << sizeInfo << ", "
                  << static_cast<long long>(bootstrapResults.mCILow * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.aCILow * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.bootstrappedValuesAverage * NANOSECONDS_IN_SECOND) << ", "
                  << static_cast<long long>(bootstrapResults.aCIHigh * NANOSECONDS_IN_SECOND) << ", "
		  << static_cast<long long>(bootstrapResults.mCIHigh * NANOSECONDS_IN_SECOND) << std::endl;

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
