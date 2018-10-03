#include <iostream>
#include <cassert>
#include <array>
#include <set>
#include <vector>
#include <random>
#include <ctime>
#include <cstring>
#include <iomanip>

template<int MAX_SIZE>
class PriorityBufferArray
{
private:
    std::array<int, MAX_SIZE> m_items;
    int itemCnt = 0;

public:
    void insert(int item)
    {
        int i = 0;
        while (i < itemCnt && item > m_items[i])
            i++;

        if (i < MAX_SIZE && m_items[i] == item)
            return;

        if (itemCnt == MAX_SIZE)
        {
            if (i == MAX_SIZE)
                i--;

            // posun dole
            for (int j = 0; j < i; j++)
                m_items[j] = m_items[j + 1];
        }
        else
        {
            // posun hore
            for (int j = itemCnt; j > i; j--)
                m_items[j] = m_items[j - 1];
        }

        m_items[i] = item;

        if (itemCnt < MAX_SIZE)
            itemCnt++;
    }

    int top()
    {
        if (itemCnt == 0)
            return -1;

        return m_items[itemCnt - 1];
    }

    void remove()
    {
        if (itemCnt == 0)
            return;

        itemCnt--;
    }
};

template<int MAX_SIZE>
class PriorityBufferSet
{
private:
    std::set<int> m_items;

public:
    void insert(int item)
    {
        if (m_items.size() == MAX_SIZE)
        {
            if (m_items.find(item) == m_items.end())
            {
                m_items.erase(*m_items.begin());
                m_items.insert(item);
            }
        }
        m_items.insert(item);
    }

    int top()
    {
        return *m_items.rbegin();
    }

    void remove()
    {
        m_items.erase(*m_items.rbegin());
    }
};

void unitTests()
{
    PriorityBufferArray<3> pba;
    pba.insert(2);
    assert(pba.top() == 2);

    pba.insert(1);
    assert(pba.top() == 2);

    pba.insert(3);
    assert(pba.top() == 3);

    pba.insert(0);
    assert(pba.top() == 3);

    pba.remove();
    assert(pba.top() == 2);

    pba.remove();
    assert(pba.top() == 0);

    PriorityBufferSet<3> pbs;
    pbs.insert(2);
    assert(pbs.top() == 2);

    pbs.insert(1);
    assert(pbs.top() == 2);

    pbs.insert(3);
    assert(pbs.top() == 3);

    pbs.insert(0);
    assert(pbs.top() == 3);

    pbs.remove();
    assert(pbs.top() == 2);

    pbs.remove();
    assert(pbs.top() == 0);
}

void benchmarkTests()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 1'000'000'000);

    const int randomNumbersCnt = 7'000'000;
    std::vector<int> randomNumbers(randomNumbersCnt);
    for (int i = 0; i < randomNumbersCnt; i++)
    {
        randomNumbers[i] = dist(rng);
    }

    clock_t beginTime, endTime;

    PriorityBufferArray<5> pba5;
    beginTime = std::clock();
    for (int x : randomNumbers)
        pba5.insert(x);
    endTime = std::clock();
    std::cout << "insert time PriorityBufferArray with n = 5 ... " <<
        (double)(endTime - beginTime) / CLOCKS_PER_SEC << " seconds " << std::endl;

    PriorityBufferSet<5> pbs5;
    beginTime = std::clock();
    for (int x : randomNumbers)
        pbs5.insert(x);
    endTime = std::clock();
    std::cout << "insert time PriorityBufferSet with n = 5 ... " <<
        (double)(endTime - beginTime) / CLOCKS_PER_SEC << " seconds " << std::endl;

    PriorityBufferArray<10> pba10;
    beginTime = std::clock();
    for (int x : randomNumbers)
        pba10.insert(x);
    endTime = std::clock();
    std::cout << "insert time PriorityBufferArray with n = 10 ... " <<
        (double)(endTime - beginTime) / CLOCKS_PER_SEC << " seconds " << std::endl;

    PriorityBufferArray<10> pbs10;
    beginTime = std::clock();
    for (int x : randomNumbers)
        pbs10.insert(x);
    endTime = std::clock();
    std::cout << "insert time PriorityBufferSet with n = 10 ... " <<
        (double)(endTime - beginTime) / CLOCKS_PER_SEC << " seconds " << std::endl;

    PriorityBufferArray<10000> pba10000;
    beginTime = std::clock();
    for (int x : randomNumbers)
        pba10000.insert(x);
    endTime = std::clock();
    std::cout << "insert time PriorityBufferArray with n = 10000 ... " <<
        (double)(endTime - beginTime) / CLOCKS_PER_SEC << " seconds " << std::endl;

    PriorityBufferSet<10000> pbs10000;
    beginTime = std::clock();
    for (int x : randomNumbers)
        pbs10000.insert(x);
    endTime = std::clock();
    std::cout << "insert time PriorityBufferSet with n = 10000 ... " <<
        (double)(endTime - beginTime) / CLOCKS_PER_SEC << " seconds " << std::endl;
}

namespace Benchmarking
{
    enum class Profiles { TIME, PRECISION } profile = Profiles::TIME;
    int timeLimit = 1; // seconds
    int precisionLimit = 20; // percent
    const int BOOTSTRAP_CYCLES_COUNT = 1;


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
                    // I dont know how this can happen yet
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
                std::cout << "measured times size = " << measuredTimes.size() << std::endl;
                long long suma = 0;
                for (unsigned j = 0; j < measuredTimes.size(); j++)
                {
                    unsigned index = std::rand() % measuredTimes.size();
                    selectedTimes.push_back(measuredTimes[index]);
                    suma += measuredTimes[index];
                    std::cout << suma << std::endl;
                }
                long long sum = std::accumulate(selectedTimes.begin(), selectedTimes.end(), 0LL);
                double average = sum / NANOSECONDS_IN_SECOND / (double)selectedTimes.size(); // estimator
                bootstrappedValues.push_back(average);
            }

            double bootstrappedValuesSum = std::accumulate(bootstrappedValues.begin(), bootstrappedValues.end(), 0.0);
            double bootstrappedValuesAverage = bootstrappedValuesSum / bootstrappedValues.size();

            std::sort(bootstrappedValues.begin(), bootstrappedValues.end());

            double mCILow = ;
            double aCILow;
            double mCIHigh;
            double aCIHigh;
            std::cout << name << ", " << size << ", " << mCILow << ", " << aCILow << ", " << bootstrappedValuesAverage <<
                         ", " << mCIHigh << ", " << aCIHigh << std::endl;
        }
        else
        {
            std::cout << "Running precision benchmark..." << std::endl;
        }


        // bootstrapping
        //std::cout << std::fixed << "elapsed time = " <<  / 10 << "s" << std::endl;
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
    std::cout << "str = " << str << std::endl;
    Benchmarking::stop();
}

int main(int argc, char* argv[])
{
    Benchmarking::init(argc, argv);
    Benchmarking::run("string benchmark", &funcToBench, 1000);

    return 0;
}

