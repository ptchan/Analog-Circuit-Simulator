/*
==========================================
 Title:  SPICE-like C++ Analog Circuit Simulator
 Author: Po Yu Chang
 Date:   1 Aug 2024
==========================================
*/

#ifndef GUASSIAN_ELIMINATION_H
#define GUASSIAN_ELIMINATION_H

void Guassian_elimination(double **matrixA, double *matrixB, int matrix_size);
void swap(double **matrixA, double *matrixB, int *number, int a, int b, int matrix_size);
#endif