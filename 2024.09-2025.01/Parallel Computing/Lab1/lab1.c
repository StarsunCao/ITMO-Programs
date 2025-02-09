#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

void selection_sort(float arr[], int len)
{
    int i, j, temp;

    for (i = 0; i < len - 1; i++)
    {
        int min = i;
        for (j = i + 1; j < len; j++)
        {
            if (arr[min] > arr[j])
            {
                min = j;
            }
        }

        if (min != i)
        {
            temp = arr[min];
            arr[min] = arr[i];
            arr[i] = temp;
        }
    }
}

int main(int argc, char *argv[])
{
    int i, N;
    struct timeval T1, T2;
    long delta_ms;
    unsigned int A = 1;
    unsigned int A2 = 1;
    unsigned int A3 = 1;

    if (argc < 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]); /* N equals the first command-line parameter */
	// printf("N = %d\n", N);

    gettimeofday(&T1, NULL); /* remember the current time T1 */
    for (i = 0; i < 50; i++) /* 50 experiments */
    {
        float sum = 0;
        srand(i);
        float *M1 = malloc(sizeof(int) * N);
        float *M2 = malloc(sizeof(int) * (N / 2));
        float *M2a = malloc(sizeof(int) * (N / 2));
        int j;

        for (j = 0; j < N; j++) {
            M1[j] = rand_r(&A2)%12 + 1;
			// printf("M1 = %f\n", M1[j]);
        }

        for (j = 0; j < N / 2; j++) {
            M2[j] = rand_r(&A3)%(9*A+1) + A;
			// printf("M2 = %f\n", M2[j]);
        }

        for (j = 0; j < N; j++) {
            M1[j] = sinh(M1[j]) * sinh(M1[j]);
			// printf("M11 = %f\n", M1[j]);
        }

        for (j = 0; j < N / 2; j++) {
            M2a[j] = M2[j];
			
        }

        for (j = 0; j < N / 2; j++) {
			// printf("mid2 = %d\n", N);
            if (j >= 1) {
                M2[j] = M2a[j - 1] + M2a[j];
				// printf("midM22223 = %f\n", M2[j]);
				
            } else {
                M2[j] = M2[j];
				// printf("midM2222 = %f\n", M2[j]);
            }
        }

        for (j = 0; j < N / 2; j++) {
            if (j >= 1) {
                M2[j] = fabsf(sin(M2a[j] + M2a[j - 1]));
				// printf("midabsM2a1 = %f\n", M2a[j]);
            } else {
                M2[j] = fabsf(sin(M2a[j]));
				// printf("midabsM2a2 = %f\n", M2a[j]);
            }
			// printf("midabsM2 = %f\n", M2[j]);
        }

        for (j = 0; j < N / 2; j++) {
			// printf("midM1 = %f\n", M1[j]);
			// printf("midM2 = %f\n", M2[j]);
            M2[j] = M1[j] * M2[j];
        }

        selection_sort(M2, N / 2);

        for (j = 0; j < N / 2; j++) {
            sum += sin(M2[j]);
			// printf("midsin = %f\n", sin(M2[j]));
			// printf("midsum = %f\n", sum);
        }
        if (M2[0] != 0) {
            sum = sum / M2[0];
        } else {
            for(j=0;j<N/2;j++){
				if (M2[j] != 0){
					sum = sum / M2[j];	
					break;
				}
			}
        }

		
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
    gettimeofday(&T2, NULL);
    /* remember the current time T2 */
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
    printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms); /* T2 - T1 */
    return 0;
}