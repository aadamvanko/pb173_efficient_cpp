#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>
#include <cmath>

using namespace std;

constexpr int MAX_LENGTH = 210000;
char str1[MAX_LENGTH];
char str2[MAX_LENGTH];

int LCSuff[2][MAX_LENGTH + 1];

/* Returns length of longest common substring of X[0..m-1]
   and Y[0..n-1] */
int LCSubStr(char *X, char *Y, int m, int n)
{
    // Create a table to store lengths of longest common suffixes of
    // substrings.   Notethat LCSuff[i][j] contains length of longest
    // common suffix of X[0..i-1] and Y[0..j-1]. The first row and
    // first column entries have no logical meaning, they are used only
    // for simplicity of program
    int result = 0;  // To store length of the longest common substring

    /* Following steps build LCSuff[m+1][n+1] in bottom up fashion. */
    for (int i=0; i<=m; i++)
    {
        for (int j=0; j<=n; j++)
        {
            if (i == 0 || j == 0) {
                if (i == 0 || i == 1) {
                    LCSuff[i][j] = 0;
                }
            }
            else if (X[i - 1] == Y[j-1])
            {
                LCSuff[1][j] = LCSuff[0][j-1] + 1;
                result = max(result, LCSuff[1][j]);
            }
            else {
                LCSuff[1][j] = 0;
            }
        }
        swap(LCSuff[0], LCSuff[1]);
    }
    return result;
}

int lcs(char a[], char b[], int aLen, int bLen, int count) {
    if (aLen == 0 || bLen == 0)
        return count;
    if (a[aLen - 1] == b[bLen - 1]) {
        count = lcs(a, b, aLen - 1, bLen - 1, count + 1);
    }
    count = max(count, max(lcs(a, b, aLen, bLen - 1, 0), lcs(a, b, aLen - 1, bLen, 0)));
    return count;
}


int main()
{
    fgets(str1, MAX_LENGTH, stdin);
    size_t str1Len = strlen(str1);
    str1[--str1Len] = '\0';
    fgets(str2, MAX_LENGTH, stdin);
    size_t str2Len = strlen(str2);
    str2[--str2Len] = '\0';

    int best = /*LCSubStr(str1, str2, str1Len, str2Len);*/lcs(str1, str2, str1Len, str2Len, 0);
    /*for (char *str1It = str1; *str1It; str1It++) {
        for (char *str2It = str2; *str2It; str2It++) {
            char *s1It = str1It;
            char *s2It = str2It;
            while (*s1It && *s2It && *s1It == *s2It) {
                s1It++;
                s2It++;
            }
            auto d = s1It - str1It;
            best = (best < d ? d : best);
        }
    }*/

    //printf("%s\n", str1);
    //printf("%s\n", str2);
    printf("%d\n", best);
    return 0;
}
