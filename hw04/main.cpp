#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::vector;

// #include "../hw01/benchmark_tool.hpp"

bool containsSubstringCheck(const string&a, const string& b)
{
    return a.find(b) != string::npos;
}

bool containsSubstringNaive(const string& a, const string& b)
{
    for (auto itA = a.cbegin(); itA != a.cend() - b.size(); itA++)
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

bool containsSubstringKMP(const string& a, const string& b)
{
    if (b.empty())
        return false;

    vector<int> pi(b.size(), 0);
    for (size_t i = 1, k = 0; i < b.size(); ++i) {
        while (k && b[k] != b[i])
            k = pi[k - 1];
        if (b[k] == b[i])
                ++k;
        pi[i] = k;
    }

    for (size_t i = 0, k = 0; i < a.size(); ++i) {
        while (k && b[k] != a[i])
            k = pi[k - 1];
        if (b[k] == a[i])
            ++k;
        if (k == b.size())
            return true;
    }

    return false;
}

// 0 reserved for bad state (index)
uint16_t states[65536][26] = { 0 };
bool containsSubstringDFA(const string& a, const string& b)
{
    // posledné dve políčka budú mať v prípade, že b.size() = 65536 veľmi veľké hodnoty pretože dôjde k pretečeniu
    for (uint16_t i = 0; i < b.size(); i++)
    {
        states[i + 1][b[i] - 'a'] = i + 2;
    }

    size_t state = 1;
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
    }
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
    const vector<char> chars{ 'a', 'b' };
    for (int lengthA = 2; lengthA <= 1024; lengthA *= 2) {
        string strA = generateString(lengthA, chars);
        string strB = generateString(lengthA / 2, chars);
        bool correctResult = containsSubstringCheck(strA, strB);
        assert(correctResult == containsSubstringNaive(strA, strB));
        assert(correctResult == containsSubstringKMP(strA, strB));
        assert(correctResult == containsSubstringDFA(strA, strB));
    }
}

int main()
{
    unitTests();

    const vector<char> chars{ 'a', 'b' };
    for (int lengthA = 2; lengthA <= 65536; lengthA *= 2) {
        string strA = generateString(lengthA, chars);
        string strB = generateString(lengthA / 2, chars);
    }
    return 0;
}
