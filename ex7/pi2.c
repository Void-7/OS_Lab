#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define NR_TOTAL 1000000
#define NR_CPU 6
#define NR_CHILD (NR_TOTAL/NR_CPU)

struct param {
    int start;
    int end;
};

struct result {
    double sum;
};

void *compute(void *arg)
{
    struct param *param;
    struct result *result;
    double sum = 0;
    int i;

    param = (struct param *)arg;
    for (i = param->start; i < param->end; i++)
        sum += (1.0/(2.0*i+1))*pow(-1,i);

    result = malloc(sizeof(struct result));
    result->sum = sum;
    return result;
}

int main()
{ 
    pthread_t workers[NR_CPU];
    struct param params[NR_CPU]; 
    int i;

    for (i = 0; i < NR_CPU; i++) {
        struct param *param;
        param = &params[i];
        param->start = i * NR_CHILD; 
        param->end = (i + 1) * NR_CHILD;
        pthread_create(&workers[i], NULL, compute, param);
    }

    double sum = 0;
    for (i = 0; i < NR_CPU; i++) {
        struct result *result;
        pthread_join(workers[i], (void **)&result);
        sum += result->sum;
        free(result);
    }
    sum*=4;
    printf("PI = %lf\n", sum);
    return 0;
}