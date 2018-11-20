#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cassert>
#include <utility>

#include "../hw01/benchmark_tool.hpp"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::to_string;

bool containsSubstringCheck(const string&a, const string& b)
{
    return a.find(b) != string::npos;
}

bool containsSubstringNaive(const string& a, const string& b)
{
    if (a.length() < b.length())
    {
        return false;
    }

    for (auto itA = a.cbegin(); itA != a.cend() - b.size() + 1; itA++)
    {
        bool ok = true;
        auto itA2 = itA;
        auto itB = b.cbegin();
        while (itB != b.cend() && ok)
        {
            ok = (*itA2 == *itB);
            itB++;
            itA2++;
        }
        if (ok)
        {
            return true;
        }
    }
    return false;
}

uint16_t kmpTable[65536];
bool containsSubstringKMP(const string& a, const string& b)
{
    if (a.length() < b.length())
    {
        return false;
    }

    if (b.empty())
        return false;

    memset(kmpTable, 0, b.size() * sizeof(uint16_t));
    for (size_t i = 1, k = 0; i < b.size(); ++i) {
        while (k && b[k] != b[i])
            k = kmpTable[k - 1];
        if (b[k] == b[i])
                ++k;
        kmpTable[i] = k;
    }

    for (size_t i = 0, k = 0; i < a.size(); ++i) {
        while (k && b[k] != a[i])
            k = kmpTable[k - 1];
        if (b[k] == a[i])
            ++k;
        if (k == b.size())
            return true;
    }

    return false;
}

bool containsSubstringDFA(const string& a, const string& b)
{
    if (a.length() < b.length())
    {
        return false;
    }

    /*size_t state = 1;
    for (const char c : a)
    {
        state = states[state][c - 'a'];
        if (state == b.size() + 1)
        {
            return true;
        }
        else if (state == 0)
        {
            state = 1;
        }
    }*/
    return false;
}

string generateString(int length, const vector<char>& chars)
{
    if (chars.empty())
    {
        return "";
    }

    std::mt19937 rng;
    rng.seed(0);
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, chars.size() - 1);

    string s;
    for (int i = 0; i < length; i++)
    {
        s.append(1, chars[dist(rng)]);
    }

    return s;
}

void unitTests()
{
    vector<std::pair<string, string>> testStrings {
        { "aaaaabbbba", "ba" },
        { "aaaaabbbba", "bc" },
        { "aaaaabbbba", "ab" },
        { "aaaaabbbba", "ab" },
        { "aaaaabbbba", "aba" },
        { "aaaaabbbba", "aa" }
    };

    for (const auto testCase : testStrings)
    {
        cout << testCase.first << " " << testCase.second << endl;
        bool correctResult = containsSubstringCheck(testCase.first, testCase.second);
        cout << "correctResult=" << correctResult << endl;
        assert(correctResult == containsSubstringNaive(testCase.first, testCase.second));
        assert(correctResult == containsSubstringKMP(testCase.first, testCase.second));
        //assert(correctResult == containsSubstringDFA(testCase.first, testCase.second));
    }

    const vector<char> chars{ 'a', 'b' };
    for (int lengthA = 2; lengthA <= 1024; lengthA *= 2) {
        string strA = generateString(lengthA, chars);
        string strB = generateString(lengthA / 2, chars);
        bool correctResult = containsSubstringCheck(strA, strB);
        assert(correctResult == containsSubstringNaive(strA, strB));
        assert(correctResult == containsSubstringKMP(strA, strB));
        //assert(correctResult == containsSubstringDFA(strA, strB));
    }
}

struct benchmarkData
{
    string a;
    string b;
};

volatile bool res;
void benchmark_naive(void* data)
{
    const benchmarkData& d = *(benchmarkData*)data;
    Benchmarking::size_info("string_length=" + to_string(d.a.length()) + ", substring_length=" + to_string(d.b.length()));
    Benchmarking::start();
    res = containsSubstringNaive(d.a, d.b);
    Benchmarking::stop();
    res ^= res;
}

void benchmark_KMP(void* data)
{
    const benchmarkData& d = *(benchmarkData*)data;
    Benchmarking::size_info("string_length=" + to_string(d.a.length()) + ", substring_length=" + to_string(d.b.length()));
    Benchmarking::start();
    res = containsSubstringKMP(d.a, d.b);
    Benchmarking::stop();
    res ^= res;
}

void benchmark_DFA(void* data)
{
    const benchmarkData& d = *(benchmarkData*)data;
    Benchmarking::size_info("string_length=" + to_string(d.a.length()) + ", substring_length=" + to_string(d.b.length()));
    Benchmarking::start();
    res = containsSubstringDFA(d.a, d.b);
    Benchmarking::stop();
    res ^= res;
}

int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);
    unitTests();

    const vector<char> chars{ 'a', 'b' };
    for (int lengthA = 2; lengthA <= 65536; lengthA *= 2) {
        benchmarkData data;
        data.a = generateString(lengthA, chars);
        data.b = generateString(lengthA / 2, chars);
        cout << data.a << endl;
        cout << data.b << endl;

        BENCHMARKING_RUN(benchmark_naive, &data);
        BENCHMARKING_RUN(benchmark_KMP, &data);
        //BENCHMARKING_RUN(benchmark_DFA, &data);
    }
    return 0;
}
