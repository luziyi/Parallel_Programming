#include <stdio.h>
#include <stdlib.h>

double calculate_sine(double radian, long max_n);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Parameters error: radian N!\n");
        exit(1);
    }

    double radian = atof(argv[1]);
    long max_n = atol(argv[2]);

    double result = calculate_sine(radian, max_n);

    printf("%lf\n", result);

    return 0;
}

double calculate_sine(double radian, long max_n) {
    double res = 0.0;
    double tmp;
    long i, j;

    for (i = 0; i <= max_n; i++) {
        tmp = 1.0;
        for (j = 1; j <= 2 * i + 1; j++) {
            tmp *= radian / j;
        }
        if (i % 2 > 0) {
            res -= tmp;
        } else {
            res += tmp;
        }
    }

    return res;
}
