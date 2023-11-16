#include <stdio.h>
#include <string.h>
#include <time.h>

void computeNext(char *pattern, int next[]);
void computeNextVal(char *pattern, int nextval[]);
int KMP(char *text, char *pattern, int next[]);

int main()
{
    char text[] = "AABABCABABCABCABABC";
    char pattern[] = "ABCABABC";

    int m = strlen(pattern);
    int next[m];
    int nextval[m];

    computeNext(pattern, next);
    computeNextVal(pattern, nextval);

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    int result = KMP(text, pattern, next);
    end = clock();
    cpu_time_used = (double)(end - start);
    printf("KMP算法匹配位置：%d\n用时%lf毫秒\n", result, cpu_time_used);

    start = clock();
    result = KMP(text, pattern, nextval);
    end = clock();
    cpu_time_used = (double)(end - start);
    printf("改进后的KMP算法匹配位置：%d\n用时%lf毫秒\n", result, cpu_time_used);

    return 0;
}

// 计算字符串next[]的函数
void computeNext(char *pattern, int next[])
{
    int m = strlen(pattern);
    int i = 0, j = -1;
    next[0] = -1;

    while (i < m)
    {
        while (j >= 0 && pattern[i] != pattern[j])
        {
            j = next[j];
        }
        i++;
        j++;
        next[i] = j;
    }
}

// 计算字符串nextval[]的函数
void computeNextVal(char *pattern, int nextval[])
{
    int m = strlen(pattern);
    int i = 0, j = -1;
    nextval[0] = -1;

    while (i < m)
    {
        while (j >= 0 && pattern[i] != pattern[j])
        {
            j = nextval[j];
        }
        i++;
        j++;

        if (i == m || pattern[i] != pattern[j])
        {
            nextval[i] = j;
        }
        else
        {
            nextval[i] = nextval[j];
        }
    }
}

// KMP算法
int KMP(char *text, char *pattern, int next[])
{
    int n = strlen(text);
    int m = strlen(pattern);
    int i = 0, j = 0;

    while (i < n)
    {
        while (j >= 0 && text[i] != pattern[j])
        {
            j = next[j];
        }
        i++;
        j++;

        if (j == m)
        {
            return i - j + 1; // 匹配成功，返回匹配位置
        }
    }

    return -1; // 没有匹配
}

