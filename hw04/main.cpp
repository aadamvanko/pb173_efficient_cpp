#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::cout;
using std::endl;
using std::vector;

#include "../hw01/benchmark_tool.hpp"

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
    for (int i = 1, k = 0; i < b.size(); ++i) {
        while (k && b[k] != b[i])
            k = pi[k - 1];
        if (b[k] == b[i])
                ++k;
        pi[i] = k;
    }

    for (int i = 0, k = 0; i < a.size(); ++i) {
        while (k && b[k] != a[i])
            k = pi[k - 1];
        if (b[k] == a[i])
            ++k;
        if (k == b.size())
            return true;
    }

    return false;
}

bool containsSubstringDFA(const string& a, const string& b)
{
    return true;
}

string generateString(int length, const vector<char>& chars)
{
    string s;

    for (int i = 0; i < length; i++)
    {

    }

    return s;
}

int main()
{
    cout << containsSubstringNaive("abbabba", "bab") << endl;
    return 0;
}
