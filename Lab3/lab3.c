#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mpi.h"

#define MATRIX_SIZE 600 // 矩阵的最大规模

// 矩阵数据结构
typedef struct
{
    int rows;
    int cols;
    double **data;
} Matrix;

void swap(Matrix **matrix_one, Matrix **matrix_two)
{
    Matrix *temp;
    temp = *matrix_one;
    *matrix_one = *matrix_two;
    *matrix_two = temp;
}

void CreateMatrix(Matrix *matrix)
{
    matrix->rows = MATRIX_SIZE;
    matrix->cols = MATRIX_SIZE;
    matrix->data = (double **)malloc(MATRIX_SIZE * sizeof(double *));
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        matrix->data[i] = (double *)malloc(MATRIX_SIZE * sizeof(double));
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            matrix->data[i][j] = rand() % 5;
        }
    }
}

void PrintMatrix(Matrix *matrix)
{
    for (int j = 0; j < matrix->cols; j++)
    {
        printf("%d ", (int)matrix->data[0][j]);
    }
}

// 矩阵乘法函数 参数，行+初始矩阵+存储结果的空间
void matrixMultiply(int n, int row, Matrix *multiply, Matrix *matrix, Matrix *result)
{
    double *temp = (double *)malloc(MATRIX_SIZE * sizeof(double));
    for (int power = 0; power < n; power++)
    {
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            double sum = 0;
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                sum += multiply->data[row][j] * matrix->data[j][i];
            }
            temp[i] = sum;
        }
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            multiply->data[row][i] = temp[i];
            result->data[row][i] = temp[i];
        }
    }
    free(temp);
}

// 计算矩阵幂函数
void matrixPower(Matrix *matrix, Matrix *multiply, Matrix *result, int n, int size, int rank)
{
    MPI_Bcast(&matrix->data[0][0], MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD); // 广播矩阵
    MPI_Barrier(MPI_COMM_WORLD);                                                              /// 同步所有进程

    int row = rank;
    while (row < MATRIX_SIZE)
    {
        matrixMultiply(n, row, multiply, matrix, result);
        row += size;
    }
}

void init(Matrix *matrix)
{
    matrix->rows = MATRIX_SIZE;
    matrix->cols = MATRIX_SIZE;
    matrix->data = (double **)malloc(MATRIX_SIZE * sizeof(double *));
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        matrix->data[i] = (double *)malloc(MATRIX_SIZE * sizeof(double));
    }
}

void freeMatrix(Matrix *matrix)
{
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        free(matrix->data[i]);
    }
    free(matrix->data);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <power> \n", argv[0]);
        return 1;
    }
    MPI_Init(&argc, &argv);
    int rank, size;
    int n = atoi(argv[1]);                // 获取幂次n
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获取当前进程的rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 获取进程总数
    // printf("Hello world from process %d of %d\n", rank, size);
    if (n < 0)
    {
        printf("Invalid input: Both power and number of threads must be positive integers.\n");
        MPI_Finalize(); // 结束MPI环境
        return 1;
    }

    Matrix *m1 = (Matrix *)malloc(sizeof(Matrix)), *m2 = (Matrix *)malloc(sizeof(Matrix)), *m3 = (Matrix *)malloc(sizeof(Matrix));

    init(m1);
    init(m2);
    init(m3);
    for (int i = 0; i < m3->rows; i++)
    {
        for (int j = 0; j < m3->cols; j++)
        {
            m3->data[i][j] = (i == j) ? 1 : 0;
        }
    }

    for (int i = 0; i < m2->rows; i++)
    {
        for (int j = 0; j < m2->cols; j++)
        {
            m2->data[i][j] = (i == j) ? 1 : 0;
        }
    }
    CreateMatrix(m1);
    matrixPower(m1, m2, m3, n, size, rank);

    // 使用MPI_SEND和MPI_Recv函数整理数据
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        if (rank == i % size)
        {
            MPI_Request request;
            MPI_Isend(m3->data[i], MATRIX_SIZE, MPI_DOUBLE, 0, i, MPI_COMM_WORLD, &request);
            MPI_Request_free(&request); // 释放发送请求，确保不会占用资源
        }
    }

    // 接收端
    if (rank == 0)
    {
        MPI_Request requests[MATRIX_SIZE]; // 为每个接收创建一个请求
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            MPI_Irecv(m3->data[i], MATRIX_SIZE, MPI_DOUBLE, i % size, i, MPI_COMM_WORLD, &requests[i]);
        }
        // 等待所有接收完成
        MPI_Waitall(MATRIX_SIZE, requests, MPI_STATUSES_IGNORE);
    }
    if (rank == 0)
    {
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                printf("%d ", (int)m3->data[i][j]);
            }
            printf("\n");
        }
    }

    freeMatrix(m1);
    freeMatrix(m2);
    freeMatrix(m3);

    MPI_Finalize(); // 结束MPI环境
    return 0;
}
