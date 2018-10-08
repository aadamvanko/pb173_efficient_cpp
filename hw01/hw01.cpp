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

namespace Benchmarking
{
    enum class Profiles { TIME, PRECISION } profile = Profiles::PRECISION;
    int timeLimit = 2; // seconds
    int precisionLimit = 2; // percent

    const int PRECISION_INITIAL_TIME_LIMIT = 5; // seconds
    const int BOOTSTRAP_CYCLES_COUNT = 10000;
    const long long NANOSECONDS_IN_SECOND = 1000000000;

    struct timespec startTime;
    struct timespec stopTime;

    std::vector<long long> measuredTimes;


    void init(int argc, char* argv[])
    {
        // for good bootstrapping
        srand(time(NULL));
        // for nice outputs
        std::cout << std::fixed << std::setprecision(10);

        if (argc != 3)
        {
            std::cout << "Wrong number of agruments given to program!" << std::endl;
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

    namespace
    {
        void run_benchmark_for_time(int limit, void (*functionToBenchmark)(int), int size)
        {
            timespec benchmarkStartTime, benchmarkEndTime;
            clock_gettime(CLOCK_MONOTONIC, &benchmarkStartTime);
            clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
            while (benchmarkEndTime.tv_sec - benchmarkStartTime.tv_sec < limit)
            {
                functionToBenchmark(size);
                long long measuredTime = (stopTime.tv_sec - startTime.tv_sec) * NANOSECONDS_IN_SECOND +
                                                 (stopTime.tv_nsec - startTime.tv_nsec);
                measuredTimes.push_back(measuredTime);
                clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
            }
        }

        void run_benchmark_once(void (*functionToBenchmark)(int), int size)
        {
            functionToBenchmark(size);
            long long measuredTime = (stopTime.tv_sec - startTime.tv_sec) * NANOSECONDS_IN_SECOND +
                                             (stopTime.tv_nsec - startTime.tv_nsec);
            measuredTimes.push_back(measuredTime);
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
            std::vector<double> bootstrappedValues;
            for (int i = 0; i < BOOTSTRAP_CYCLES_COUNT; i++)
            {
                std::vector<long long> selectedTimes;
                long long suma = 0;
                for (unsigned j = 0; j < measuredTimes.size(); j++)
                {
                    unsigned index = std::rand() % measuredTimes.size();
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
            double sd = 0.0;
            for (double bootstrappedValue : bootstrappedValues)
            {
                sd += (bootstrappedValue - bootstrappedValuesAverage) * (bootstrappedValue - bootstrappedValuesAverage);
            }
            sd /= bootstrappedValues.size();
            sd = std::sqrt(sd);

            double mCILow = bootstrappedValues[unsigned(bootstrappedValues.size() * 0.05)];
            double aCILow = bootstrappedValuesAverage - 2 * sd;
            double mCIHigh = bootstrappedValues[unsigned(bootstrappedValues.size() * 0.95)];
            double aCIHigh = bootstrappedValuesAverage + 2 * sd;
            return { mCILow, aCILow, mCIHigh, aCIHigh, bootstrappedValuesAverage };
        }
    }

    void run(const char* name, void (*functionToBenchmark)(int), int size)
    {
        measuredTimes.clear();
        BootstrappingResults bootstrapResults;

        if (profile == Profiles::TIME)
        {
            std::cout << "Running time benchmark..." << std::endl;
            run_benchmark_for_time(timeLimit, functionToBenchmark, size);

            std::cout << "Bootstrapping..." << std::endl;
            bootstrapResults = calculate_bootstrap_stats();
        }
        else
        {
            std::cout << "Running precision benchmark..." << std::endl;
            run_benchmark_for_time(PRECISION_INITIAL_TIME_LIMIT, functionToBenchmark, size);

            bootstrapResults = calculate_bootstrap_stats();
            std::cout << "CI wanted diff = " << precisionLimit / 100.0 << std::endl;
            std::cout << "CI width = " << bootstrapResults.mCIHigh / bootstrapResults.mCILow - 1 << std::endl;
            while (bootstrapResults.mCIHigh / bootstrapResults.mCILow >= (1.0 + precisionLimit / 100.0))
            {
                std::cout << "Not enough precision, running one more benchmark..." << std::endl;
                run_benchmark_once(functionToBenchmark, size);
                bootstrapResults = calculate_bootstrap_stats();
                std::cout << "CI width = " << bootstrapResults.mCIHigh / bootstrapResults.mCILow - 1<< std::endl;
            }
        }

        std::cout << name << ", " << size << ", "
                  << bootstrapResults.mCILow << ", "
                  << bootstrapResults.aCILow << ", "
                  << bootstrapResults.bootstrappedValuesAverage << ", "
                  << bootstrapResults.mCIHigh << ", "
                  << bootstrapResults.aCIHigh << std::endl;
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

void funcToBench(int times)
{
    Benchmarking::start();
    std::string str;
    for (int i = 0; i < times; i++)
    {
        str += "a";
    }
    str.erase(str.begin() + 1, str.end());
    times = str.size();
    Benchmarking::stop();
}

int main(int argc, char* argv[])
{
    Benchmarking::init(argc, argv);
    Benchmarking::run("string benchmark", &funcToBench, 100000000);

    return 0;
}

