#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

double res;

typedef struct Args {
    float radian;
	long max_n;
	long begin;
	long step;
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
	Args *args = (Args *)malloc(threads * sizeof(Args));
	pthread_t *pid = (pthread_t *)malloc(threads * sizeof(pthread_t));
	
	res = 0.0;
	for (int i = 0; i < threads; i++) {
        args[i].radian = radian;
		args[i].max_n = max_n;
		args[i].begin = i;
		args[i].step = threads;
		pthread_create(&pid[i], NULL, cal, (void *)&args[i]);
	}
	for (int i = 0; i < threads; i++) {
		pthread_join(pid[i], NULL);
	}
	printf("%lf\n", res);
	free(pid);
	free(args);
	return 0;
}

void *cal(void *_arg) {
	long j;
	double val, tmp;
	Args *arg = (Args *)_arg;
	
	val = 0.0;
	for (long i = arg->begin; i <= arg->max_n; i += arg->step) {
        tmp = 1.0;
		for (j = 1; j <= 2*i+1; j++) {
            tmp *= arg->radian / j;
        }
        if (i % 2 > 0) {
            val -= tmp;
        } else {
            val += tmp;
        }
	}
	res += val;
	return NULL;
}
