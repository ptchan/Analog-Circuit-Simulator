/*
==========================================
 Title:  SPICE-like C++ Analog Circuit Simulator
 Author: Po Yu Chang
 Date:   1 Aug 2024
==========================================
*/

#ifndef TRAPEZOIDAL_ANALYSIS_H
#define TRAPEZOIDAL_ANALYSIS_H

void Trapezoidal_analysis(double **matrixA, double *matrixB, double **matrix_nonlinear_components, double **matrix_pulse_voltage, 
double **matrix_switch, int **matrix_diode, int **matrix_mosfet, int matrix_size, int node_number, double time_h, int nonlinear_components, int pulse_voltage,
int switch_number, int diode_number, int mosfet_number, int total_steps);

#endif