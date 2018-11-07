#include <iostream>
#include <bitset>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <string>
#include <memory>

#include "../hw01/benchmark_tool.hpp"

using std::uint16_t;
using std::cout;
using std::endl;

constexpr unsigned MAX_SIZE = 65536;

class set_bit_vector
{
private:
    std::bitset<MAX_SIZE> data;

public:
    void insert(uint16_t value)
    {
        data[value] = true;
    }

    void erase(uint16_t value)
    {
        data[value] = false;
    }

    set_bit_vector makeUnion(const set_bit_vector& other)
    {
        set_bit_vector result;
        for (size_t i = 0; i < data.size(); i++)
        {
            result.data[i] = data[i] || other.data[i];
        }
        return result;
    }

    set_bit_vector makeIntersection(const set_bit_vector& other)
    {
        set_bit_vector result;
        for (size_t i = 0; i < data.size(); i++)
        {
            result.data[i] = data[i] && other.data[i];
        }
        return result;
    }

    bool operator[](size_t index)
    {
        return data[index];
    }

    void print(std::ostream& os)
    {
        std::ostringstream bits;
        bits << data;
        std::string naturalOrder(bits.str());
        std::reverse(naturalOrder.begin(), naturalOrder.end());
        os << naturalOrder << endl;
    }
};

class set_nibble_trie
{
private:
    static constexpr int MAX_CHILDREN = 16;
    static constexpr int MAX_LEVEL = 4;

    struct Node
    {
        std::unique_ptr<Node> children[MAX_CHILDREN];
        bool isLeaf = false;
    } root;

    void addRecursive(const Node* node, set_nibble_trie& result, uint16_t path)
    {
        if (node->isLeaf)
        {
            uint16_t reversed = ((path & 0xF000) >> 12) |
                                ((path & 0x0F00) >> 4) |
                                ((path & 0x00F0) << 4) |
                                ((path & 0x000F) << 12);
            result.insert(reversed);
        }
        else
        {
            for (uint16_t i = 0; i < MAX_CHILDREN; i++)
            {
                if (node->children[i] != nullptr)
                {
                    addRecursive(node->children[i].get(), result, (path << 4) + i);
                }
            }
        }
    }

public:
    void insert(uint16_t value)
    {
        Node* node = &root;
        for (int level = 0; level < MAX_LEVEL; level++)
        {
            uint16_t byte = value & 0xF;
            value >>= 4;
            if (node->children[byte] == nullptr)
            {
                node->children[byte] = std::make_unique<Node>();
                node->children[byte]->isLeaf = (level == MAX_LEVEL - 1);
            }
            node = node->children[byte].get();
        }
    }

    bool contains(uint16_t value)
    {
        const Node* node = &root;
        while (node != nullptr && !node->isLeaf)
        {
            uint16_t byte = value & 0xF;
            value >>= 4;
            node = node->children[byte].get();
        }
        return node != nullptr;
    }

    set_nibble_trie makeUnion(const set_nibble_trie& other)
    {
        set_nibble_trie result;
        addRecursive(&root, result, 0);
        addRecursive(&other.root, result, 0);
        return result;
    }
};

void set_bit_vector_unit_tests(bool debug=false)
{
    set_bit_vector sbv1;
    sbv1.insert(0);
    sbv1.insert(2);
    sbv1.insert(65535);
    assert(sbv1[0]);
    assert(sbv1[2]);
    if (debug) sbv1.print(cout);

    set_bit_vector sbv2;
    sbv2.insert(0);
    sbv2.insert(1);
    assert(sbv2[0]);
    assert(sbv2[1]);
    if (debug) sbv2.print(cout);

    auto unionRes = sbv1.makeUnion(sbv2);
    assert(unionRes[0]);
    assert(unionRes[1]);
    assert(unionRes[2]);
    assert(unionRes[65535]);
    if (debug) unionRes.print(cout);

    auto intersectionRes = sbv1.makeIntersection(sbv2);
    assert(intersectionRes[0]);
    if (debug) intersectionRes.print(cout);

    set_bit_vector sbv3;
    sbv3.insert(0);
    sbv3.insert(1);
    sbv3.insert(65535);
    assert(sbv3[0]);
    assert(sbv3[1]);
    assert(sbv3[65535]);
    sbv3.erase(0);
    sbv3.erase(1);
    sbv3.erase(65535);
    assert(!sbv3[0]);
    assert(!sbv3[1]);
    assert(!sbv3[65535]);
}

void set_nibble_trie_unit_tests(bool debug=false)
{
    set_nibble_trie snt1;
    snt1.insert(0);
    snt1.insert(2);
    snt1.insert(65535);
    assert(snt1.contains(0));
    assert(snt1.contains(2));
    assert(snt1.contains(65535));
    //if (debug) snt1.print(cout);

    set_nibble_trie snt2;
    snt2.insert(0);
    snt2.insert(1);
    assert(snt2.contains(0));
    assert(snt2.contains(1));
    //if (debug) snt2.print(cout);

    auto unionRes = snt1.makeUnion(snt2);
    assert(unionRes.contains(0));
    assert(unionRes.contains(1));
    assert(unionRes.contains(2));
    assert(unionRes.contains(65535));
    //if (debug) unionRes.print(cout);
}

void union_unit_tests()
{
    std::mt19937 rng;
    rng.seed(0);
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 65535);

    const int randomNumbersCnt = 1000;
    std::vector<uint16_t> randomNumbersA(randomNumbersCnt);
    std::vector<uint16_t> randomNumbersB(randomNumbersCnt);
    for (int i = 0; i < randomNumbersCnt; i++)
    {
        randomNumbersA[i] = dist(rng);
        randomNumbersB[i] = dist(rng);
    }

    set_bit_vector sbv1;
    set_bit_vector sbv2;
    set_nibble_trie snt1;
    set_nibble_trie snt2;
    for (const auto number : randomNumbersA)
    {
        sbv1.insert(number);
        snt1.insert(number);
    }
    for (const auto number : randomNumbersB)
    {
        sbv2.insert(number);
        snt2.insert(number);
    }

    auto bitVectorUnionRes = sbv1.makeUnion(sbv2);
    auto bitNibbleTrieUnionRes = snt1.makeUnion(snt2);

    for (uint16_t i = 0; i < 65535; i++)
    {
        assert(bitVectorUnionRes[i] == bitNibbleTrieUnionRes.contains(i));
    }
}

struct benchmark_data_inserts
{
    std::vector<uint16_t> numbers;
};

struct benchmark_data_union
{
    std::vector<uint16_t> numbersA;
    std::vector<uint16_t> numbersB;
};

void benchmark_set_bit_vector_inserts(void* data)
{
    const std::vector<uint16_t> numbers = ((benchmark_data_inserts*)data)->numbers;
    Benchmarking::size_info("numbers_count=" + std::to_string(numbers.size()));
    set_bit_vector sbv;
    Benchmarking::start();
    for (const auto number : numbers)
    {
        sbv.insert(number);
    }
    Benchmarking::stop();
    volatile bool c = sbv[0];
}

void benchmark_set_nibble_trie_inserts(void* data)
{
    const std::vector<uint16_t> numbers = ((benchmark_data_inserts*)data)->numbers;
    Benchmarking::size_info("numbers_count=" + std::to_string(numbers.size()));
    set_nibble_trie snt;
    Benchmarking::start();
    for (const auto number : numbers)
    {
        snt.insert(number);
    }
    Benchmarking::stop();
    volatile bool c = snt.contains(0);
}

void benchmark_set_bit_vector_union(void* data)
{
    const std::vector<uint16_t> numbersA = ((benchmark_data_union*)data)->numbersA;
    const std::vector<uint16_t> numbersB = ((benchmark_data_union*)data)->numbersB;
    Benchmarking::size_info("numbers_count=" + std::to_string(numbersA.size()));
    set_bit_vector sbvA;
    set_bit_vector sbvB;
    for (const auto number : numbersA) sbvA.insert(number);
    for (const auto number : numbersB) sbvB.insert(number);
    Benchmarking::start();
    auto unionRes = sbvA.makeUnion(sbvB);
    Benchmarking::stop();
    volatile bool c = unionRes[0];
}

void benchmark_set_nibble_trie_union(void* data)
{
    const std::vector<uint16_t> numbersA = ((benchmark_data_union*)data)->numbersA;
    const std::vector<uint16_t> numbersB = ((benchmark_data_union*)data)->numbersB;
    Benchmarking::size_info("numbers_count=" + std::to_string(numbersA.size()));
    set_nibble_trie sntA;
    set_nibble_trie sntB;
    for (const auto number : numbersA) sntA.insert(number);
    for (const auto number : numbersB) sntB.insert(number);
    Benchmarking::start();
    auto unionRes = sntA.makeUnion(sntB);
    Benchmarking::stop();
    volatile bool c = unionRes.contains(0);
}

void benchmark_set_bit_vector_intersection(void* data)
{
    const std::vector<uint16_t> numbersA = ((benchmark_data_union*)data)->numbersA;
    const std::vector<uint16_t> numbersB = ((benchmark_data_union*)data)->numbersB;
    Benchmarking::size_info("numbers_count=" + std::to_string(numbersA.size()));
    set_bit_vector sbvA;
    set_bit_vector sbvB;
    for (const auto number : numbersA) sbvA.insert(number);
    for (const auto number : numbersB) sbvB.insert(number);
    Benchmarking::start();
    auto intersectionRes = sbvA.makeIntersection(sbvB);
    Benchmarking::stop();
    volatile bool c = intersectionRes[0];
}

int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);

    set_bit_vector_unit_tests();
    set_nibble_trie_unit_tests();
    union_unit_tests();

    // prepare random numbers for benchmarking
    std::mt19937 rng;
    auto seedValue = std::random_device()();
    rng.seed(seedValue);
    //cout << "seedValue=" << seedValue << endl;
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 65535);

    for (int randomNumbersCount = 2; randomNumbersCount <= 2048; randomNumbersCount *= 4) {
        std::vector<uint16_t> randomNumbersA(randomNumbersCount);
        std::vector<uint16_t> randomNumbersB(randomNumbersCount);
        for (int i = 0; i < randomNumbersCount; i++)
        {
            randomNumbersA[i] = dist(rng);
            randomNumbersB[i] = dist(rng);
        }

        benchmark_data_inserts benchmarkDataInserts{ randomNumbersA };
        BENCHMARKING_RUN(benchmark_set_bit_vector_inserts, &benchmarkDataInserts);
        BENCHMARKING_RUN(benchmark_set_nibble_trie_inserts, &benchmarkDataInserts);

        benchmark_data_union benchmarkDataUnion{ randomNumbersA, randomNumbersB };
        BENCHMARKING_RUN(benchmark_set_bit_vector_union, &benchmarkDataUnion);
        BENCHMARKING_RUN(benchmark_set_nibble_trie_union, &benchmarkDataUnion);

        // BENCHMARKING_RUN(benchmark_set_bit_vector_intersection, &benchmarkDataUnion);
    }



    return 0;
}
