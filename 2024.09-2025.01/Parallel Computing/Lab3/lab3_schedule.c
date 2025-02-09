#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h> // 添加 OpenMP 头文件

void selection_sort(float arr[], int len) {
    int i, j, temp;

    #pragma omp parallel for schedule(runtime) private(i, j, temp)
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

    if (argc < 3) {
        printf("Usage: %s <N> <threads>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]); // 数据规模
    threads = atoi(argv[2]); // 使用的线程数

    omp_set_num_threads(threads); // 动态设置线程数

    gettimeofday(&T1, NULL); // 记录开始时间 T1

    for (i = 0; i < 50; i++) {
        float sum = 0;
        float *M1 = malloc(sizeof(float) * N);
        float *M2 = malloc(sizeof(float) * (N / 2));
        float *M2a = malloc(sizeof(float) * (N / 2));
        int j;

        unsigned int seeds[threads];
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            seeds[tid] = i + tid; // 每个线程的独立种子
        }

        // 初始化 M1 数组
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N; j++) {
            int tid = omp_get_thread_num();
            M1[j] = rand_r(&seeds[tid]) % 12 + 1;
        }

        // 初始化 M2 数组
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N / 2; j++) {
            int tid = omp_get_thread_num();
            M2[j] = rand_r(&seeds[tid]) % 9 + 1;
        }

        // 计算 M1 的 sinh 平方值
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N; j++) {
            M1[j] = sinh(M1[j]) * sinh(M1[j]);
        }

        // 复制 M2 到 M2a
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N / 2; j++) {
            M2a[j] = M2[j];
        }

        // 更新 M2 数据
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N / 2; j++) {
            if (j >= 1) {
                M2[j] = M2a[j - 1] + M2a[j];
            }
        }

        // 计算 M2 的 fabs(sin)
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N / 2; j++) {
            if (j >= 1) {
                M2[j] = fabsf(sin(M2a[j] + M2a[j - 1]));
            } else {
                M2[j] = fabsf(sin(M2a[j]));
            }
        }

        // 更新 M2
        #pragma omp parallel for schedule(runtime) private(j)
        for (j = 0; j < N / 2; j++) {
            M2[j] = M1[j] * M2[j];
        }

        // 排序（单线程保证结果一致性）
        selection_sort(M2, N / 2);

        // 计算 sum
        #pragma omp parallel for reduction(+:sum) schedule(runtime) private(j)
        for (j = 0; j < N / 2; j++) {
            sum += sin(M2[j]);
        }

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

        // 偶数检查
        if ((int)sum % 2 == 0) {
            sum = sum;
        } else {
            sum = sum + 1;
        }

        printf("x = %d\n", (int)sum);

        free(M1);
        free(M2);
        free(M2a);
    }

    gettimeofday(&T2, NULL); // 记录结束时间 T2
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
    printf("\nN=%d, Threads=%d. Milliseconds passed: %ld\n", N, threads, delta_ms);

    return 0;
}
