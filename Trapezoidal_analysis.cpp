/*
==========================================
 Title:  SPICE-like C++ Analog Circuit Simulator
 Author: Po Yu Chang
 Date:   1 Aug 2024
==========================================
*/

#include "Trapezoidal_analysis.h"
#include "Guassian_elimination.h"
#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

void Trapezoidal_analysis(double **matrixA, double *matrixB, double **matrix_nonlinear_components, double **matrix_pulse_voltage,
double **matrix_switch, int **matrix_diode, int **matrix_mosfet, int matrix_size, int node_number, double time_h, int nonlinear_components, int pulse_voltage, 
int switch_number, int diode_number, int mosfet_number, int total_steps){

    //MOSFET parameter
    double L = 180 * pow(10,-9);
    double W = 360 * pow(10,-9);
    double un = 450 * pow(10,-4);
    double Cox = 8.6 * pow(10,-3);
    double VA = 0.9;
    double Vt = 0.48;

    ofstream outFile2("graph.txt");
    outFile2 << matrix_size << endl;
    outFile2 << node_number << endl;
    outFile2 << total_steps << endl;

    double **output_matrix = new double *[matrix_size];
    double *matrixB_replicate = new double[matrix_size];
    double *result = new double[matrix_size];
    double **matrixA_replicate = new double *[matrix_size];
    const double pi = acos(-1);
    for (int i = 0; i < matrix_size; i++){
        matrixA_replicate[i] = new double[matrix_size];
        output_matrix[i] = new double[total_steps];
        result[i] = 0;
    }

    for (int i = 0; i < matrix_size; i++){
        matrixB_replicate[i] = matrixB[i];
        for (int j = 0; j < matrix_size; j++){
            matrixA_replicate[i][j] = matrixA[i][j];
        }
        for (int k = 0; k < matrix_size; k++){
            output_matrix[i][k] = 0;
        }
    }

    // Guassian elimination one by one
    for (int i = 0; i < total_steps; i++){
        for (int j = 0; j < pulse_voltage; j++){
            if (int(matrix_pulse_voltage[j][0]) == 1){
                if (i == 0) {
                    matrixB[node_number + int(matrix_pulse_voltage[j][1]) - 1] = matrixB[node_number + int(matrix_pulse_voltage[j][1]) - 1] + matrix_pulse_voltage[j][2];
                }
                else if (int(matrix_pulse_voltage[j][6]) % int(matrix_pulse_voltage[j][7]) <= (matrix_pulse_voltage[j][4] / 0.0000001)) {
                    matrixB[node_number + int(matrix_pulse_voltage[j][1]) - 1] = matrixB[node_number + int(matrix_pulse_voltage[j][1]) - 1] + matrix_pulse_voltage[j][3];
                }
                matrix_pulse_voltage[j][6] = matrix_pulse_voltage[j][6] + 1;
            }
            else if (int(matrix_pulse_voltage[j][0]) == 2){
                int count = int(matrix_pulse_voltage[j][6]) % int(matrix_pulse_voltage[j][7]);
                matrixB[node_number + int(matrix_pulse_voltage[j][1]) - 1] = matrixB[node_number + int(matrix_pulse_voltage[j][1]) - 1] + matrix_pulse_voltage[j][2] + matrix_pulse_voltage[j][3]*sin(2*pi*count/matrix_pulse_voltage[j][7] + matrix_pulse_voltage[j][5]); 
                matrix_pulse_voltage[j][6] = matrix_pulse_voltage[j][6] + 1;
            }
        }

        for (int j = 0; j < switch_number; j++){
            if ((result[int(matrix_switch[j][2]) - 1] - result[int(matrix_switch[j][3]) - 1]) > matrix_switch[j][6]){
                if (matrix_switch[j][0] == 0 || matrix_switch[j][1] == 0){
                    int num = matrix_switch[j][0] == 0 ? matrix_switch[j][1] : matrix_switch[j][0];
                    matrixA[num-1][num-1] = matrixA[num-1][num-1] + 1/matrix_switch[j][4];
                }
                else{
                    matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][1]) - 1] = matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][1]) - 1] - 1/matrix_switch[j][4];
                    matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][0]) - 1] = matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][0]) - 1] - 1/matrix_switch[j][4];
                    matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][0]) - 1] = matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][0]) - 1] + 1/matrix_switch[j][4];
                    matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][1]) - 1] = matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][1]) - 1] + 1/matrix_switch[j][4];
                }
            }
            else{
                if (matrix_switch[j][0] == 0 || matrix_switch[j][1] == 0){
                    int num = matrix_switch[j][0] == 0 ? matrix_switch[j][1] : matrix_switch[j][0];
                    matrixA[num-1][num-1] = matrixA[num-1][num-1] + 1/matrix_switch[j][5];
                }
                else{
                    matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][1]) - 1] = matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][1]) - 1] - 1/matrix_switch[j][5];
                    matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][0]) - 1] = matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][0]) - 1] - 1/matrix_switch[j][5];
                    matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][0]) - 1] = matrixA[int(matrix_switch[j][0]) - 1][int(matrix_switch[j][0]) - 1] + 1/matrix_switch[j][5];
                    matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][1]) - 1] = matrixA[int(matrix_switch[j][1]) - 1][int(matrix_switch[j][1]) - 1] + 1/matrix_switch[j][5];
                }
            }
        }

        for (int j = 0; j < diode_number; j++){
            if (matrix_diode[j][0] == 0){
                if (result[int(matrix_diode[j][1]) - 1] <= 0){
                    matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] = matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] + 1;
                }
                else {
                    matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] = matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] + 1*pow(10,-9);
                }
            }
            else if(matrix_diode[j][1] == 0){
                if (result[int(matrix_diode[j][0]) - 1] <= 0){
                    matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] = matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] + 1*pow(10,-9);
                }
                else {
                    matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] = matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] + 1;
                }
            }
            else {
                if (result[int(matrix_diode[j][0]) - 1] >= result[int(matrix_diode[j][1]) - 1]){
                    matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] = matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] + 1;
                    matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] = matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] + 1;
                    matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][0]) - 1] = matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][0]) - 1] - 1;
                    matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][1]) - 1] = matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][1]) - 1] - 1;
                }
                else {
                    matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] = matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][0]) - 1] + 1*pow(10,-9);
                    matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] = matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][1]) - 1] + 1*pow(10,-9);
                    matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][0]) - 1] = matrixA[int(matrix_diode[j][1]) - 1][int(matrix_diode[j][0]) - 1] - 1*pow(10,-9);
                    matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][1]) - 1] = matrixA[int(matrix_diode[j][0]) - 1][int(matrix_diode[j][1]) - 1] - 1*pow(10,-9);
                }
            }
        }

        for (int j = 0; j < mosfet_number; j++){
            int drain = (matrix_mosfet[j][0] == 0) ? 0 : result[int(matrix_mosfet[j][0]) - 1];
            int gate = (matrix_mosfet[j][1] == 0) ? 0 : result[int(matrix_mosfet[j][1]) - 1];
            int source = (matrix_mosfet[j][2] == 0) ? 0 : result[int(matrix_mosfet[j][2]) - 1];
            if (gate - source < Vt){
                if (matrix_mosfet[j][0] == 0){
                    matrixA[int(matrix_mosfet[j][2]) - 1][int(matrix_mosfet[j][2]) - 1] = matrixA[int(matrix_mosfet[j][2]) - 1][int(matrix_mosfet[j][2]) - 1] + 1*pow(10,-9);
                }
                else if (matrix_mosfet[j][2] == 0){
                    matrixA[int(matrix_mosfet[j][0]) - 1][int(matrix_mosfet[j][0]) - 1] = matrixA[int(matrix_mosfet[j][0]) - 1][int(matrix_mosfet[j][0]) - 1] + 1*pow(10,-9);
                }
                else {
                    matrixA[int(matrix_mosfet[j][0]) - 1][int(matrix_mosfet[j][0]) - 1] = matrixA[int(matrix_mosfet[j][0]) - 1][int(matrix_mosfet[j][0]) - 1] + 1*pow(10,-9);
                    matrixA[int(matrix_mosfet[j][2]) - 1][int(matrix_mosfet[j][2]) - 1] = matrixA[int(matrix_mosfet[j][2]) - 1][int(matrix_mosfet[j][2]) - 1] + 1*pow(10,-9);
                    matrixA[int(matrix_mosfet[j][0]) - 1][int(matrix_mosfet[j][2]) - 1] = matrixA[int(matrix_mosfet[j][0]) - 1][int(matrix_mosfet[j][2]) - 1] - 1*pow(10,-9);
                    matrixA[int(matrix_mosfet[j][2]) - 1][int(matrix_mosfet[j][0]) - 1] = matrixA[int(matrix_mosfet[j][2]) - 1][int(matrix_mosfet[j][0]) - 1] - 1*pow(10,-9);
                }
            }
            else {
                double current;
                if (drain >= gate - Vt){
                    current = (1/2) * un * Cox * (W/L) * (gate - source - Vt) * (gate - source - Vt) * (1 + (drain - source)/VA);
                }
                else {
                    current = un * Cox * (W/L) * ((gate - source - Vt)*(drain - source) - (drain - source)*(drain - source)/2);
                }
                matrixB[int(matrix_mosfet[j][0]) - 1] = matrixB[int(matrix_mosfet[j][0]) - 1] - current;
                matrixB[int(matrix_mosfet[j][2]) - 1] = matrixB[int(matrix_mosfet[j][2]) - 1] + current;
            }
        }

        Guassian_elimination(matrixA, matrixB, matrix_size);

        if (i == total_steps - 1){
            for (int j = 0; j < matrix_size; j++){
                output_matrix[j][total_steps-1] = matrixB[j];
            }
        }
        else{
            for (int k = 0; k < matrix_size; k++){
                result[k] = matrixB[k];
                matrixB[k] = matrixB_replicate[k];
                for (int j = 0; j < matrix_size; j++){
                    matrixA[k][j] = matrixA_replicate[k][j];
                }
            }
            for (int k = 0; k < matrix_size; k++){
                output_matrix[k][i] = result[k];
            }
            for (int j = 0; j < nonlinear_components; j++){
                if (matrix_nonlinear_components[j][0] == 1){
                    if ((int(matrix_nonlinear_components[j][2]) != 0) && (int(matrix_nonlinear_components[j][3]) != 0)){
                        double vk_1 = result[int(matrix_nonlinear_components[j][2]) - 1] - result[int(matrix_nonlinear_components[j][3]) - 1];
                        double ik_1 = 2*matrix_nonlinear_components[j][1]*vk_1/time_h - matrix_nonlinear_components[j][4];
                        matrix_nonlinear_components[j][4] = 2*matrix_nonlinear_components[j][1]*vk_1/time_h + ik_1;
                        matrixB[int(matrix_nonlinear_components[j][2]) - 1] = matrixB[int(matrix_nonlinear_components[j][2]) - 1] + matrix_nonlinear_components[j][4];
                        matrixB[int(matrix_nonlinear_components[j][3]) - 1] = matrixB[int(matrix_nonlinear_components[j][3]) - 1] - matrix_nonlinear_components[j][4];
                    }
                    else {
                        double vk_1 = result[int(matrix_nonlinear_components[j][2]) - 1];
                        double ik_1 = 2*matrix_nonlinear_components[j][1]*vk_1/time_h - matrix_nonlinear_components[j][4];
                        matrix_nonlinear_components[j][4] = 2*matrix_nonlinear_components[j][1]*vk_1/time_h + ik_1;
                        matrixB[int(matrix_nonlinear_components[j][2]) - 1] = matrixB[int(matrix_nonlinear_components[j][2]) - 1] + matrix_nonlinear_components[j][4];
                    }
                }
                else if (matrix_nonlinear_components[j][0] == 2){
                    if ((int(matrix_nonlinear_components[j][2]) != 0) && (int(matrix_nonlinear_components[j][3]) != 0)){
                        double vk_1 = result[int(matrix_nonlinear_components[j][2]) - 1] - result[int(matrix_nonlinear_components[j][3]) - 1];
                        double ik_1 = vk_1*time_h/(2*matrix_nonlinear_components[j][1]) + matrix_nonlinear_components[j][4];
                        matrix_nonlinear_components[j][4] = time_h*vk_1/(2*matrix_nonlinear_components[j][1]) + ik_1;
                        matrixB[int(matrix_nonlinear_components[j][2]) - 1] = matrixB[int(matrix_nonlinear_components[j][2]) - 1] - matrix_nonlinear_components[j][4];
                        matrixB[int(matrix_nonlinear_components[j][3]) - 1] = matrixB[int(matrix_nonlinear_components[j][3]) - 1] + matrix_nonlinear_components[j][4];
                    }
                    else {
                        double vk_1 = result[int(matrix_nonlinear_components[j][2]) - 1];
                        double ik_1 = vk_1*time_h/(2*matrix_nonlinear_components[j][1]) + matrix_nonlinear_components[j][4];
                        matrix_nonlinear_components[j][4] = time_h*vk_1/(2*matrix_nonlinear_components[j][1]) + ik_1;
                        matrixB[int(matrix_nonlinear_components[j][2]) - 1] = matrixB[int(matrix_nonlinear_components[j][2]) - 1] - matrix_nonlinear_components[j][4];
                    }
                }
            }
        }
    }

    for (int i = 0; i < matrix_size; i++){
        for (int j = 0; j < total_steps; j++){
            outFile2 << output_matrix[i][j] << endl;
        }
    }
    outFile2.close();
}
