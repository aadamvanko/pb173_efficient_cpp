#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

constexpr int MAX = 7370000;
int nums[MAX + 5];

int main(int argc, char** argv)
{
    if (argc != 2) return 0;
    const int n = std::atoi(argv[1]);

    nums[0] = 1;
    nums[1] = 1;
    /*for (int i = 2; i < MAX; i++) {
        if (nums[0] == 1) continue;

        for (int j = i + i; j < MAX; j += i)
            nums[j] = 1;
    }*/

    //memset(nums, 0, sizeof(nums));
    //const long long n2 = MAX;
    for (int p=2; p*p <= MAX; p++)
    {
        // If prime[p] is not changed, then it is a prime
        if (nums[p] == 0)
        {
            // Update all multiples of p
            for (int i=p*2; i<=MAX; i += p)
                nums[i] = 1;
        }
    }

    /*for (int i = 2; i < MAX; i++)
        if (nums[i] == 0)
            cout << i << endl;*/

   int index = 0;
   for (int i = 2; i < MAX; i++) {
       if (nums[i] == 0)
           index++;
       if (index == n)
       {
            printf("%d\n", i);
            break;
       }
   }

    return 0;
}
