#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cassert>
#include <utility>
#include <regex>

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

int f[65536];
int kmp(const char* t, const char* p) {
  int m = strlen(p);
  int n = strlen(t);

  memset(f, 0, sizeof(int) * m);
  int i = 0;
  int j = 0;

  while (i < n) {
    if (t[i] == p[j]) {
      if (j == m - 1) {
        return true;
      }
      else {
        i += 1;
        j += 1;
      }
    }
    else {
      if (j > 0) {
        j = f[j-1];
      }
      else {
        i += 1;
      }
    }
  }

  return false;
}

bool containsSubstringKMP(const string& a, const string& b)
{
    if (a.length() < b.length())
    {
        return false;
    }

    return kmp(a.c_str(), b.c_str());
}

constexpr int NUMBER_OF_CHARS = 26;

/**
 * Space may be further minimized at the cost of running time
 */
unsigned DFA[65536][NUMBER_OF_CHARS];


bool DFAStringMatching(const string& text, const string& pattern){
    unsigned m = pattern.length();
    unsigned n = text.length();

    /**
     * i is incrementing by 1, meaning using text[i] whole input string can be traversed.
     * text[i] is a character. DFA[text[i]][j] means text[i] character column and j th row.
     * This position or cell contains the column for next transition.
     */
    unsigned i, j;
    for(i = 0, j = 0; i < n && j < m; ++i)
        j = DFA[j][text[i] - 'a'];

    /**
     * If j equals m then all transition completed successfully
     * meaning the string are a match.
     */
    return j == m;
}

void CreateDFA(const string& pattern){
    unsigned m = pattern.length();

    memset(DFA, 0, sizeof(unsigned) * NUMBER_OF_CHARS * m);
    /**
     * Set the first state to 1
     */
    DFA[0][pattern[0] - 'a'] = 1;

    unsigned x, j, k;
    for(x = 0, j = 1; j < m; ++j){
        /**
         * Copy all values from x column to j column.
         */
        for(k = 0; k < NUMBER_OF_CHARS; ++k)
            DFA[j][k] = DFA[x][k];

        /**
         * Update position in table to the next transition.
         */
        DFA[j][pattern[j] - 'a'] = j + 1;

        /**
         * Update the column from which to copy values.
         */
        x = DFA[x][pattern[j] - 'a'];
    }
}

bool containsSubstringDFA(const string& a, const string& b)
{
    if (a.length() < b.length())
    {
        return false;
    }

    CreateDFA(b);
    return DFAStringMatching(a, b);
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
//        cout << testCase.first << " " << testCase.second << endl;
        bool correctResult = containsSubstringCheck(testCase.first, testCase.second);
//        cout << "correctResult=" << correctResult << endl;
        assert(correctResult == containsSubstringNaive(testCase.first, testCase.second));
        assert(correctResult == containsSubstringKMP(testCase.first, testCase.second));
        assert(correctResult == containsSubstringDFA(testCase.first, testCase.second));
    }

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
//        cout << data.a << endl;
//        cout << data.b << endl;

        BENCHMARKING_RUN(benchmark_naive, &data);
        BENCHMARKING_RUN(benchmark_KMP, &data);
        BENCHMARKING_RUN(benchmark_DFA, &data);
    }
    return 0;
}
