#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

double res;

typedef struct Args {
    double radian;
    long max_n;
    long begin;
    long end;
} Args;

void *cal(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Parameters error: radian N threads!\n");
        exit(1);
    }

    double radian = atof(argv[1]);
    long max_n = atol(argv[2]);
    int threads = atoi(argv[3]);
    Args *arg;
    pthread_t *pid;
    pid = (pthread_t *)malloc(threads * sizeof(pthread_t)); 
    
    res = 0.0;
    for (int i = 0; i < threads; i++) {
        arg = (Args *)malloc(sizeof(Args));
        arg->radian = radian;
        arg->max_n = max_n;
        arg->begin = i * (max_n / threads);
        arg->end = (i == threads - 1) ? max_n : (i + 1) * (max_n / threads);
        pthread_create(&pid[i], NULL, cal, (void *)arg);
    }
    for (int i = 0; i < threads; i++) {
        pthread_join(pid[i], NULL);
    }
    printf("%lf\n", res);
    free(pid);
    return 0;
}

void *cal(void *_arg) {
    Args *arg = (Args *)_arg;
    double val = 0.0;
    for (long i = arg->begin; i < arg->end; i++) {
        double term = 1.0;
        for (long j = 1; j <= 2 * i + 1; j++) {
            term *= arg->radian / j;
        }
        val += (i % 2 == 0) ? term : -term;
    }
    res+=val;
    return NULL;
}
