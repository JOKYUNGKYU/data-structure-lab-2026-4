#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

int main(void)
{
    int test_case;
    int T;

    //freopen("input.txt", "r", stdin);  

    scanf("%d", &T);

    for (test_case = 1; test_case <= T; ++test_case)
    {
        int N;
        scanf("%d", &N);

        for (int i = 1; i <= N; i++)
        {
            int num = i;
            int clap = 0;

            while (num > 0)
            {
                int digit = num % 10;
                if (digit == 3 || digit == 6 || digit == 9)
                {
                    clap++;
                }
                num /= 10;
            }

            if (clap == 0)
            {
                printf("%d", i);
            }
            else
            {
                for (int j = 0; j < clap; j++)
                {
                    printf("-");
                }
            }

            printf(" ");
        }

        printf("\n");
    }

    return 0;
}