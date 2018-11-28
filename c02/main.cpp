#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

constexpr int LIM = 1000000;
double nums[LIM][2];

double calcDiff(const double a, const int cnt) {
    double diff = 0;
    for (int i = 0; i < cnt; i++) {
        double d = (nums[i][1] - a*nums[i][0]*nums[i][0]*nums[i][0]) * (nums[i][1] - a*nums[i][0]*nums[i][0]*nums[i][0]);
        diff += d;
    }
    return diff;
}

int main()
{
    int cnt = 0;
    while (scanf("%lf %lf", &nums[cnt][0], &nums[cnt][1]) == 2) {
        ++cnt;
    }

    double low = -100;
    double lowDiff = calcDiff(low, cnt);
    double high = 100;
    double highDiff = calcDiff(high, cnt);
    //printf("low %f high %f\n", lowDiff, highDiff);

    double a = low;
    double aDiff = calcDiff(a, cnt);
    /*double old_diff = calcDiff(a, cnt);
    double diff = 0;
*/
    for (double val = low; val < high; val += 0.0001) {
        double diff = calcDiff(val, cnt);
        //printf("new diff %f new a %f\n", diff, val);
        if (aDiff > diff) {

            aDiff = diff;
            a = val;
        }
    }
    //printf("%f\n", a);

    double lowInt = a;
    double lowIntDiff = aDiff;
    double highInt = 0;
    double highIntDiff = calcDiff(highInt, cnt);
    bool first = true;
    double diff = 0;
    double old_diff = 0;
    while (first || fabs(old_diff - diff) > 0.00001) {
        first = false;
        old_diff = diff;
        double mid = (lowInt + highInt) / 2;
        diff = calcDiff(mid, cnt);
        if (lowDiff < highDiff) {

        }
        else {

        }
        if (diff > old_diff) {
            highInt = mid;
        } else {
            lowInt = mid;
        }
    }
    printf("%f\n", lowInt);

    /*
    for (int i = 0; i < cnt; i++) {
        printf("%f %f\n", nums[i][0], nums[i][1]);
    }
*/
    return 0;
}
