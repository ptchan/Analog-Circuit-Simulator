/*
==========================================
 Title:  SPICE-like C++ Analog Circuit Simulator
 Author: Po Yu Chang
 Date:   1 Aug 2024
==========================================
*/

#include "Guassian_elimination.h"
#include <iostream>
using namespace std;

void Guassian_elimination(double **matrixA, double *matrixB, int matrix_size){
    int *number = new int[matrix_size];
    for (int i = 0; i < matrix_size; i++){
        number[i] = 0;
        for (int j = 0; j < matrix_size; j++){
            if (matrixA[i][j] != 0){
                number[i] ++;
            }
        }
    }

    for (int j = 0; j < matrix_size; j++){
        for (int i = matrix_size - 1; i > 0; i--){
            if (number[i] < number[i-1]){
                int num = number[i];
                number[i] = number[i-1];
                number[i-1] = num;

                double num1 = matrixB[i];
                matrixB[i] = matrixB[i-1];
                matrixB[i-1] = num1;

                for (int j = 0; j < matrix_size; j++){
                    double num2 = matrixA[i][j];
                    matrixA[i][j] = matrixA[i-1][j];
                    matrixA[i-1][j] = num2;
                }
            }
            else {
                i = -1;
            }
        }
    }

    for (int i = 0; i < matrix_size; i++){
        if (matrixA[i][i] == 0){
            for (int j = i + 1; j < matrix_size; j++){
                if (matrixA[j][i] != 0){
                    swap(matrixA, matrixB, number, i, j, matrix_size);
                    j = matrix_size;
                    i --;
                }
            }
        }
        else {
            for (int j = i + 1; j < matrix_size; j++){
                double num = matrixA[j][i] / matrixA[i][i];
                matrixB[j] = matrixB[j] - matrixB[i] * num;
                for (int k = 0; k < matrix_size; k++){
                    matrixA[j][k] = matrixA[j][k] - matrixA[i][k] * num;
                }
                matrixA[j][i] = 0;
            }
        }
    }

    for (int i = matrix_size - 1; i > -1; i--){
        for (int j = i - 1; j > -1; j--){
            double num = matrixA[j][i] / matrixA[i][i];
            matrixB[j] = matrixB[j] - matrixB[i] * num;
            for (int k = 0; k < matrix_size; k++){
                matrixA[j][k] = matrixA[j][k] - matrixA[i][k] * num;
            }
            matrixA[j][i] = 0;
        }
    }

    for (int i = 0; i < matrix_size; i++){
        matrixB[i] = matrixB[i] / matrixA[i][i];
        matrixA[i][i] = 1;
    }
};

// swap two lines
void swap(double **matrixA, double *matrixB, int *number, int a, int b, int matrix_size){
    int num = number[a];
    number[a] = number[b];
    number[b] = num;

    double num1 = matrixB[a];
    matrixB[a] = matrixB[b];
    matrixB[b] = num1;

    for (int j = 0; j < matrix_size; j++){
        double num2 = matrixA[a][j];
        matrixA[a][j] = matrixA[b][j];
        matrixA[b][j] = num2;
    }
};