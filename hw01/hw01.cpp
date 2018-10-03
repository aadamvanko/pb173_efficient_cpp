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
    enum class Profiles { TIME, PRECISION } profile = Profiles::TIME;
    int timeLimit = 1; // seconds
    int precisionLimit = 20; // percent
    const int BOOTSTRAP_CYCLES_COUNT = 10000;

    const double NANOSECONDS_IN_SECOND = 1e9;
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

    void run(const char* name, void (*functionToBenchmark)(int), int size)
    {
        measuredTimes.clear();

        if (profile == Profiles::TIME)
        {
            std::cout << "Running time benchmark..." << std::endl;
            timespec benchmarkStartTime, benchmarkEndTime;
            clock_gettime(CLOCK_MONOTONIC, &benchmarkStartTime);
            clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
            while (benchmarkEndTime.tv_sec - benchmarkStartTime.tv_sec < timeLimit)
            {
                functionToBenchmark(size);
                __syscall_slong_t measuredTime = stopTime.tv_nsec - startTime.tv_nsec;
                if (measuredTime < 0)
                {
                    // I dont know how this can happen yet, but it happen only with last run
                    std::cout << "ERROR benchmarked time is NEGATIVE, = " << measuredTime << std::endl;
                }
                else
                {
                    measuredTimes.push_back(measuredTime);
                }
                clock_gettime(CLOCK_MONOTONIC, &benchmarkEndTime);
            }

            std::cout << "Bootstrapping..." << std::endl;
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
                double average = sum / NANOSECONDS_IN_SECOND / (double)selectedTimes.size(); // estimator
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
            std::cout << name << ", " << size << ", " << mCILow << ", " << aCILow << ", " << bootstrappedValuesAverage <<
                         ", " << mCIHigh << ", " << aCIHigh << std::endl;
        }
        else
        {
            std::cout << "Running precision benchmark..." << std::endl;
        }
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
    Benchmarking::run("string benchmark", &funcToBench, 1000000);

    return 0;
}

