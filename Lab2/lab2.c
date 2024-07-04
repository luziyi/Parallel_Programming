#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MATRIX_SIZE 350

#define CACHE_LINE_SIZE 64 // 缓存行大小为 64 字节
typedef struct
{
    int rows;
    int cols;
    double **data;
    char padding[CACHE_LINE_SIZE - sizeof(int) * 2 - sizeof(double **)]; // 填充数据
} Matrix;

// Define thread argument structure
typedef struct
{
    Matrix *result;
    Matrix *matrix;
    Matrix *temp;
    int start_row;
    int end_row;
    int step;
    int id;
    pthread_mutex_t *mutex;
} ThreadArgs;

// Function to allocate memory for a matrix
Matrix *createMatrix(int rows, int cols)
{
    Matrix *mat = (Matrix *)malloc(sizeof(Matrix));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = (double **)malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++)
    {
        mat->data[i] = (double *)malloc(cols * sizeof(double));
    }
    return mat;
}
void printMatrix(Matrix *mat);
// Function to initialize a matrix with random values
void initMatrix(Matrix *mat)
{
    for (int i = 0; i < mat->rows; i++)
    {
        for (int j = 0; j < mat->cols; j++)
        {
            mat->data[i][j] = rand() % 10; // Random values for demonstration
        }
    }
}

// Function to free memory allocated for a matrix
void freeMatrix(Matrix *mat)
{
    for (int i = 0; i < mat->rows; i++)
    {
        free(mat->data[i]);
    }
    free(mat->data);
    free(mat);
}

// 线程函数，计算矩阵相乘
void *multiply(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    int n = args->id;
    int row;
    int colum;
    double sum;
    while (n < MATRIX_SIZE * MATRIX_SIZE)
    {
        // printf("%d\n",n);
        row = n / MATRIX_SIZE;
        colum = n % MATRIX_SIZE;
        sum = 0;
        for (int k = 0; k < MATRIX_SIZE; k++)
        {
            sum += args->temp->data[row][k] * args->matrix->data[k][colum];
        }
        pthread_mutex_lock(args->mutex);
        args->result->data[row][colum] = sum; // 将结果存入result矩阵
        pthread_mutex_unlock(args->mutex);
        n += args->step;
    }
    pthread_exit(NULL);
}

// Function to compute matrix power
Matrix *matrixPower(Matrix *matrix, int power, int num_threads)
{
    // 初始化矩阵
    Matrix *temp = createMatrix(matrix->rows, matrix->cols);
    Matrix *result = createMatrix(matrix->rows, matrix->cols);
    Matrix *swap; // 矩阵neg用于交换
    // 初始化temp矩阵用于第一次矩阵自乘
    for (int i = 0; i < temp->rows; i++)
    {
        for (int j = 0; j < temp->cols; j++)
        {
            if (i == j)
            {
                temp->data[i][j] = 1;
            }
            else
            {
                temp->data[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < power; i++)
    {
        pthread_t threads[num_threads];
        ThreadArgs thread_args[num_threads];
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

        // 划分任务创建线程
        int rows_per_thread = result->rows / num_threads;
        for (int i = 0; i < num_threads; i++)
        {
            thread_args[i].step = num_threads;
            thread_args[i].id = i;
            thread_args[i].result = result; // 专用于举证相乘的结果保存
            thread_args[i].matrix = matrix; // 为源矩阵，每次相乘中的一个
            thread_args[i].mutex = &mutex;
            thread_args[i].temp = temp; // 上一次矩阵相乘的结果，每次矩阵相乘中的一个
            pthread_create(&threads[i], NULL, multiply, (void *)&thread_args[i]);
        }

        // 等待本次相乘的所有线程运行完毕
        for (int i = 0; i < num_threads; i++)
        {
            pthread_join(threads[i], NULL);
        }
        // printMatrix(result);
        // printf("\n");
        swap = temp;
        temp = result;
        result = swap;
        // printMatrix(temp);
    }
    freeMatrix(result);
    // printMatrix(result);
    return temp;
}

// 矩阵输出，调试用
void printMatrix(Matrix *mat)
{
    for (int i = 0; i < mat->rows; i++)
    {
        for (int j = 0; j < mat->cols; j++)
        {
            printf("%.2f\t", mat->data[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <power> <num_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);           // 获取幂次n
    int num_threads = atoi(argv[2]); // 获取线程数num_threads

    if (n <= 0 || num_threads <= 0)
    {
        printf("Invalid input: Both power and number of threads must be positive integers.\n");
        return 1;
    }

    Matrix *matrix = createMatrix(MATRIX_SIZE, MATRIX_SIZE);
    initMatrix(matrix);

    // 计算矩阵幂次
    Matrix *result = matrixPower(matrix, n, num_threads);

    // 输出结果矩阵
    // printf("Result matrix after %d power:\n", n);
    // printMatrix(result);
    // 释放内存
    freeMatrix(matrix);
    freeMatrix(result);

    return 0;
}
