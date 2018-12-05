#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>

using namespace std;

constexpr int MAX_LENGTH = 100000;
char str1[MAX_LENGTH];
char str2[MAX_LENGTH];

int main()
{
    fgets(str1, MAX_LENGTH, stdin);
    size_t str1Len = strlen(str1);
    str1[--str1Len] = '\0';
    fgets(str2, MAX_LENGTH, stdin);
    size_t str2Len = strlen(str2);
    str2[--str2Len] = '\0';

    int best = 0;
    for (char *str1It = str1; *str1It; str1It++) {
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
    }

    //printf("%s\n", str1);
    //printf("%s\n", str2);
    printf("%d\n", best);
    return 0;
}
