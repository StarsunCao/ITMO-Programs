#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <fwBase.h>
#include <fwSignal.h> // 添加 AMD Framewave 的头文件

void selection_sort(float arr[], int len) {
    int i, j, temp;
    for (i = 0; i < len - 1; i++) {
        int min = i;
        for (j = i + 1; j < len; j++) {
            if (arr[min] > arr[j]) {
                min = j;
            }
        }
        if (min != i) {
            temp = arr[min];
            arr[min] = arr[i];
            arr[i] = temp;
        }
    }
}

int main(int argc, char *argv[]) {
    int i, N, threads;
    struct timeval T1, T2;
    long delta_ms;
    unsigned int A = 1;
    unsigned int A2 = 1;
    unsigned int A3 = 1;

    if (argc < 3) {
        printf("Usage: %s <N> <threads>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    threads = atoi(argv[2]);

    // Framewave 不需要手动设置线程数

    gettimeofday(&T1, NULL);  // 记录当前时间 T1
    for (i = 0; i < 50; i++) {
        float sum = 0;
        srand(i);
        float *M1 = (float *)malloc(sizeof(float) * N);
        float *M2 = (float *)malloc(sizeof(float) * (N / 2));
        float *M2a = (float *)malloc(sizeof(float) * (N / 2));
        int j;

        for (j = 0; j < N; j++) {
            M1[j] = rand_r(&A2) % 12 + 1;
        }

        for (j = 0; j < N / 2; j++) {
            M2[j] = rand_r(&A3) % (9 * A + 1) + A;
        }

        // 使用 Framewave 库的向量化 sinh 函数替代
        fwsSinh_32f_A11(M1, M1, N);

        // 复制 M2 的内容到 M2a
        for (j = 0; j < N / 2; j++) {
            M2a[j] = M2[j];
        }

        for (j = 0; j < N / 2; j++) {
            if (j >= 1) {
                M2[j] = M2a[j - 1] + M2a[j];
            }
        }

        // 使用 Framewave 库的向量化 sin 函数替代
        fwsSin_32f_A11(M2a, M2, N / 2);

        for (j = 0; j < N / 2; j++) {
            M2[j] = M1[j] * M2[j];
        }

        selection_sort(M2, N / 2);

        // 使用 Framewave 库的向量化 sin 函数计算 sum
        fwsSin_32f_A11(M2, M2, N / 2);
        fwsSum_32f(M2, N / 2, &sum, fwAlgHintAccurate);

        // 特殊情况处理
        if (M2[0] != 0) {
            sum /= M2[0];
        } else {
            for (j = 0; j < N / 2; j++) {
                if (M2[j] != 0) {
                    sum /= M2[j];
                    break;
                }
            }
        }

        if ((int)sum % 2 == 0) {
            sum = sum;
        } else {
            sum = sum + 1;
        }

        // printf("x = %d\n", (int)sum);

        free(M1);
        free(M2);
        free(M2a);
    }

    gettimeofday(&T2, NULL);
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
    printf("\nN=%d, Threads=%d. Milliseconds passed: %ld\n", N, threads, delta_ms);

    return 0;
}
