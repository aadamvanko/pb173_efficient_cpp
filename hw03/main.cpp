#include <iostream>
#include <bitset>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <string>

#include "../hw01/benchmark_tool.hpp"

using std::uint16_t;
using std::cout;
using std::endl;

constexpr unsigned MAX_SIZE = 32;

class set_bitvector
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

    set_bitvector makeUnion(const set_bitvector& other)
    {
        set_bitvector result;
        for (size_t i = 0; i < data.size(); i++)
        {
            result.data[i] = data[i] || other.data[i];
        }
        return result;
    }

    set_bitvector makeIntersection(const set_bitvector& other)
    {
        set_bitvector result;
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

void set_bitvector_unit_tests(bool debug=false)
{
    set_bitvector sbv1;
    sbv1.insert(0);
    sbv1.insert(2);
    assert(sbv1[0]);
    assert(sbv1[2]);
    if (debug) sbv1.print(cout);

    set_bitvector sbv2;
    sbv2.insert(0);
    sbv2.insert(1);
    assert(sbv2[0]);
    assert(sbv2[1]);
    if (debug) sbv2.print(cout);

    auto unionRes = sbv1.makeUnion(sbv2);
    assert(unionRes[0]);
    assert(unionRes[1]);
    assert(unionRes[2]);
    if (debug) unionRes.print(cout);

    auto intersectionRes = sbv1.makeIntersection(sbv2);
    assert(intersectionRes[0]);
    if (debug) intersectionRes.print(cout);
}

int main()
{
    set_bitvector_unit_tests();

    return 0;
}
