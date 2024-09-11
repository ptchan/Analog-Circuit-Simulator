/*
==========================================
 Title:  SPICE-like C++ Analog Circuit Simulator
 Author: Po Yu Chang
 Date:   1 Aug 2024
==========================================
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <set>
#include <math.h>

#include "Guassian_elimination.h"
#include "Trapezoidal_analysis.h"

using namespace std;

struct Component {
    string name;
    double value;
    int node1;
    int node2;
    int aux_node1;
    int aux_node2;
    double Vinitial;    // or DC offset
    double Von;   // or Amplitude
    double Ton;   // or Frequency
    double Tperiod;    // or Phase shift
    string model_name;
};

struct Model {
    int node1;
    int node2;
    int aux_node1;
    int aux_node2;
    double Ron;
    double Roff;
    double Vt;
};

class componentSortCriterion {
    public:
        bool operator() (const Component &a, const Component &b) const {
            if(a.name < b.name){
                return true;
            }
            else{
                return false;
            }
        }
};

class modelSortCriterion {
    public:
        bool operator() (const Model &a, const Model &b) const {
            if(a.node1 < b.node2){
                return true;
            }
            else{
                return false;
            }
        }
};

// Convert unit to number
double unitconverter(string x){
    double output = 0; 
    char unit = x.back();
    if (unit == 'T'){
        x.pop_back();
        output = stod(x) * pow(10,12);
    }
    else if (unit == 'G'){
        x.pop_back();
        output = stod(x) * pow(10,9);
    }
    else if (unit == 'g'){
        x.pop_back();
        output = stod(x) * pow(10,6);
    }
    else if (unit == 'k'){
        x.pop_back();
        output = stod(x) * pow(10,3);
    }
    else if (unit == 'm'){
        x.pop_back();
        output = stod(x) * pow(10,-3);
    }
    else if (unit == 'u'){
        x.pop_back();
        output = stod(x) * pow(10,-6);
    }
    else if (unit == 'n'){
        x.pop_back();
        output = stod(x) * pow(10,-9);
    }
    else if (unit == 'p'){
        x.pop_back();
        output = stod(x) * pow(10,-12);
    }
    else if (unit == 'f'){
        x.pop_back();
        output = stod(x) * pow(10,-15);
    }
    else{
        output = stod(x);
    }
    return output;
}

int main()
{
    int node_number = 0;
    int source_number = 0;
    int component_number = 0;
    int nonlinear_components = 0;
    int matrix_size = 0;
    int independent_voltage_source_number = 0;
    int pulse_voltage_source_number = 0;
    int switch_number = 0;
    double tran_time = 0;
    int diode_number = 0;
    int mosfet_number = 0;
    const double pi = acos(-1);

    set<Component, componentSortCriterion> ComponentSet;
    set<Model, modelSortCriterion> ModelSet;
    set<int> nodeSet;
    Component element;
    Model model;
    string x;

    ifstream inFile("netlist.txt");
    if (!inFile) {
        cout << "error opening source file." << endl;
        return 0;
    }
    ofstream outFile("out.txt");
    
    if (!outFile) {
        inFile.close(); 
        cout << "error opening destination file." << endl;
        return 0;
    }
    
    // parse netlist
    bool reading_data = false;
    int reader_state = 0;
    int operation_state = 0;
    int controlled_source_state = 0;
    int pulse_source_state = 0;
    int switch_state = 0;
    int model_state = 0;
    int diode_state = 0;
    int mosfet_state = 0;

    while (inFile >> x){
        if (x[0] == 'R' && (!reading_data)){
            element.name = x;
            element.aux_node1 = 0;
            element.aux_node2 = 0;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            reader_state ++;
            component_number ++;
            reading_data = true;
        }
        else if (x[0] == 'V' && (!reading_data)){
            element.name = x;
            element.aux_node1 = 0;
            element.aux_node2 = 0;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            source_number ++;
            reader_state ++;
            component_number ++;
            independent_voltage_source_number ++;
            reading_data = true;
        }
        else if (x[0] == 'I' && (!reading_data)){
            element.name = x;
            element.aux_node1 = 0;
            element.aux_node2 = 0;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            reader_state ++;
            component_number ++;
            reading_data = true;
        }
        else if (x[0] == 'C' && (!reading_data)){
            element.name = x;
            element.aux_node1 = 0;
            element.aux_node2 = 0;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            reader_state ++;
            component_number ++;
            nonlinear_components ++;
            reading_data = true;
        }
        else if (x[0] == 'L' && (!reading_data)){
            element.name = x;
            element.aux_node1 = 0;
            element.aux_node2 = 0;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            reader_state ++;
            component_number ++;
            nonlinear_components ++;
            reading_data = true;
        }
        else if (x[0] == 'E' && (!reading_data)){
            element.name = x;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            controlled_source_state ++;
            component_number ++;
            source_number ++;
            reading_data = true;
        }
        else if (x[0] == 'G' && (!reading_data)){
            element.name = x;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            controlled_source_state ++;
            component_number ++;
            reading_data = true;
        }
        else if (x[0] == 'S' && (!reading_data)){
            element.name = x;
            element.model_name = " ";
            component_number ++;
            switch_state ++;
            switch_number ++;
            reading_data = true;
        }
        else if (switch_state == 1){
            element.node1 = stoi(x);
            nodeSet.insert(element.node1);
            switch_state ++;
        }
        else if (switch_state == 2){
            element.node2 = stoi(x);
            nodeSet.insert(element.node2);
            switch_state ++;
        }
        else if (switch_state == 3){
            element.aux_node1 = stoi(x);
            switch_state ++;
        }
        else if (switch_state == 4){
            element.aux_node2 = stoi(x);
            switch_state ++;
        }
        else if (switch_state == 5){
            element.model_name = x;
            switch_state = 0;
            reading_data = false;
            ComponentSet.insert(element);
        }
        else if (reader_state == 1){
            element.node1 = stoi(x);
            nodeSet.insert(element.node1);
            reader_state ++;
        }
        else if (reader_state == 2){
            element.node2 = stoi(x);
            nodeSet.insert(element.node2);
            reader_state ++;
        }
        else if (reader_state == 3){
            if (x == "PULSE"){
                pulse_source_state ++;
                pulse_voltage_source_number ++;
                reader_state = 0;
                element.model_name = x;
            }
            else if (x == "SINE"){
                pulse_source_state ++;
                pulse_voltage_source_number ++;
                reader_state = 0;
                element.model_name = x;
            }
            else{
                element.value = unitconverter(x);
                reader_state = 0;
                ComponentSet.insert(element);
                reading_data = false;
            }
        }
        else if (pulse_source_state == 1){
            element.Vinitial = unitconverter(x);
            pulse_source_state ++;
        }
        else if (pulse_source_state == 2){
            element.Von = unitconverter(x);
            pulse_source_state ++;
        }
        else if (pulse_source_state == 3){
            element.Ton = unitconverter(x);
            pulse_source_state ++;
        }
        else if (pulse_source_state == 4){
            element.Tperiod = unitconverter(x);
            pulse_source_state = 0;
            ComponentSet.insert(element);
            reading_data = false;
        }

        else if (controlled_source_state == 1){
            element.node1 = stoi(x);
            nodeSet.insert(element.node1);
            controlled_source_state ++;
        }
        else if (controlled_source_state == 2){
            element.node2 = stoi(x);
            nodeSet.insert(element.node2);
            controlled_source_state ++;
        }
        else if (controlled_source_state == 3){
            element.aux_node1 = stoi(x);
            controlled_source_state ++;
        }
        else if (controlled_source_state == 4){
            element.aux_node2 = stoi(x);
            controlled_source_state ++;
        }
        else if (controlled_source_state == 5){
            element.value = stod(x);
            controlled_source_state = 0;
            ComponentSet.insert(element);
            reading_data = false;
        }
        else if (operation_state == 1){
            operation_state = 0;
            tran_time = unitconverter(x);
        }
        else if (x == ".tran" && (!reading_data)){
            operation_state ++;
        }
        else if (x == ".model" && (!reading_data)){
            model_state ++;
            reading_data = true;
        }
        else if (model_state == 1){
            set<Component, componentSortCriterion>::iterator iter;
            iter = ComponentSet.begin();
            for (int i = 0; i < component_number; i++){
                if ((*iter).model_name == x){
                    model.node1 = (*iter).node1;
                    model.node2 = (*iter).node2;
                    model.aux_node1 = (*iter).aux_node1;
                    model.aux_node2 = (*iter).aux_node2;
                    i = component_number;
                }
                iter ++;
            }
            model_state ++;
        }
        else if (model_state == 2){
            model_state ++;
        }
        else if (model_state == 3){
            model.Ron = unitconverter(x);
            model_state ++;
        }
        else if (model_state == 4){
            model.Roff = unitconverter(x);
            model_state ++;
        }
        else if (model_state == 5){
            model.Vt = unitconverter(x);
            ModelSet.insert(model);
            model_state = 0;
            reading_data = false;
        }
        else if (x[0] == 'D' && (!reading_data)){
            element.name = x;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            diode_state ++;
            component_number ++;
            diode_number ++;
            reading_data = true;
        }
        else if (diode_state == 1){
            element.node1 = stoi(x);
            nodeSet.insert(element.node1);
            diode_state ++;
        }
        else if (diode_state == 2){
            element.node2 = stoi(x);
            nodeSet.insert(element.node2);
            diode_state = 0;
            reading_data = false;
            ComponentSet.insert(element);
        }
        else if (x[0] == 'M' && (!reading_data)){
            element.name = x;
            element.Vinitial = 0;
            element.Von = 0;
            element.Ton = 0;
            element.Tperiod = 0;
            element.model_name = " ";
            mosfet_state ++;
            component_number ++;
            mosfet_number ++;
            reading_data = true;
        }
        else if (mosfet_state == 1){
            element.node1 = stoi(x);
            nodeSet.insert(element.node1);
            mosfet_state ++;
        }
        else if (mosfet_state == 2){
            element.node2 = stoi(x);
            nodeSet.insert(element.node2);
            mosfet_state ++;
        }
        else if (mosfet_state == 3){
            element.aux_node1 = stoi(x);
            nodeSet.insert(element.aux_node1);
            mosfet_state ++;
        }
        else if (mosfet_state == 4){
            mosfet_state = 0;
            reading_data = false;
            ComponentSet.insert(element);
        }
        else if (x == ".end"){
            inFile.close();
        }
    }
    
    node_number = nodeSet.size() - 1;
    matrix_size = node_number + source_number;
    set<Component, componentSortCriterion>::iterator iter;
    iter = ComponentSet.begin();

    double *matrixB = new double[matrix_size];
    double **matrixA = new double *[matrix_size];
    double **matrix_nonlinear_components = new double *[nonlinear_components];
    double **matrix_pulse_voltage = new double *[pulse_voltage_source_number];
    double **matrix_switch = new double *[switch_number];
    int **matrix_mosfet = new int *[mosfet_number];
    int **matrix_diode = new int *[diode_number];

    for (int i = 0; i < matrix_size; i++){
        matrixA[i] = new double[matrix_size];
    }
    for (int i = 0; i < nonlinear_components; i++){
        matrix_nonlinear_components[i] = new double[5];
    }
    for (int i = 0; i < pulse_voltage_source_number; i++){
        matrix_pulse_voltage[i] = new double[8];
    }
    for (int i = 0; i < switch_number; i++){
        matrix_switch[i] = new double[7];
    }
    for (int i = 0; i < diode_number; i++){
        matrix_diode[i] = new int[2];
    }
    for (int i = 0; i < mosfet_number; i++){
        matrix_mosfet[i] = new int[3];
    }

    // initialization
    for (int i = 0; i < matrix_size; i++){
        for (int j = 0; j < matrix_size; j++){
            matrixA[i][j] = 0;
        }
        matrixB[i] = 0;
    }
    for (int i = 0; i < nonlinear_components; i++){
        for (int j = 0; j < 5; j++){
            matrix_nonlinear_components[i][j] = 0;
        }
    }
    for (int i = 0; i < pulse_voltage_source_number; i++){
        for (int j = 0; j < 8; j++){
            matrix_pulse_voltage[i][j] = 0;
        }
    }
    for (int i = 0; i < switch_number; i++){
        for (int j = 0; j < 7; j++){
            matrix_switch[i][j] = 0;
        }
    }
    for (int i = 0; i < diode_number; i++){
        for (int j = 0; j < 2; j++){
            matrix_diode[i][j] = 0;
        }
    }
    for (int i = 0; i < mosfet_number; i++){
        for (int j = 0; j < 3; j++){
            matrix_mosfet[i][j] = 0;
        }
    }

    double time_h = 0.0000001;
    int total_steps = tran_time / time_h;

    // stamp the matrix    
    iter = ComponentSet.begin();
    int nonlinear_components_count = 0;
    int pulse_voltage_count = 0;
    int model_count = 0;
    int diode_count = 0;
    int mosfet_count = 0;

    set<Model, modelSortCriterion>::iterator iter2;
    iter2 = ModelSet.begin();
    for (int i = 0; i < switch_number; i++){
        matrix_switch[i][0] = (*iter2).node1;
        matrix_switch[i][1] = (*iter2).node2;
        matrix_switch[i][2] = (*iter2).aux_node1;
        matrix_switch[i][3] = (*iter2).aux_node2;
        matrix_switch[i][4] = (*iter2).Ron;
        matrix_switch[i][5] = (*iter2).Roff;
        matrix_switch[i][6] = (*iter2).Vt;
        iter2 ++;
    }

    for (int i = 0; i < component_number; i++){
        if ((*iter).name[0] == 'R'){
            if ((*iter).node1 == 0 || (*iter).node2 == 0){
                int num = (*iter).node1 == 0 ? (*iter).node2 : (*iter).node1;
                matrixA[num-1][num-1] = matrixA[num-1][num-1] + 1/(*iter).value;
            }
            else{
                matrixA[(*iter).node1 - 1][(*iter).node2 - 1] = matrixA[(*iter).node1 - 1][(*iter).node2 - 1] - 1/(*iter).value;
                matrixA[(*iter).node2 - 1][(*iter).node1 - 1] = matrixA[(*iter).node2 - 1][(*iter).node1 - 1] - 1/(*iter).value;
                matrixA[(*iter).node1 - 1][(*iter).node1 - 1] = matrixA[(*iter).node1 - 1][(*iter).node1 - 1] + 1/(*iter).value;
                matrixA[(*iter).node2 - 1][(*iter).node2 - 1] = matrixA[(*iter).node2 - 1][(*iter).node2 - 1] + 1/(*iter).value;
            }
        }
        else if ((*iter).name[0] == 'V'){
            string name = (*iter).name;
            name.erase(name.begin());
            int num = stoi(name);
            if ((*iter).node1 != 0){
                matrixA[(*iter).node1 - 1][node_number + num - 1] = 1;
                matrixA[node_number + num - 1][(*iter).node1 - 1] = 1;
            }
            if ((*iter).node2 != 0){
                matrixA[(*iter).node2 - 1][node_number + num - 1] = -1;
                matrixA[node_number + num - 1][(*iter).node2 - 1] = -1;
            }
            if ((*iter).model_name == "PULSE"){
                matrix_pulse_voltage[pulse_voltage_count][0] = 1;   // 1 represents pulse voltage source
                matrix_pulse_voltage[pulse_voltage_count][1] = num; // represents the number of the voltage
                matrix_pulse_voltage[pulse_voltage_count][2] = (*iter).Vinitial;    // represents Vinitial
                matrix_pulse_voltage[pulse_voltage_count][3] = (*iter).Von; // represents Von
                matrix_pulse_voltage[pulse_voltage_count][4] = (*iter).Ton; // represents Ton
                matrix_pulse_voltage[pulse_voltage_count][5] = (*iter).Tperiod;     // represents Tperiod
                matrix_pulse_voltage[pulse_voltage_count][7] = (*iter).Tperiod / 0.0000001;
                // matrix_pulse_voltage[pulse_voltage_count][6] represents counter
                pulse_voltage_count ++;
            }
            else if ((*iter).model_name == "SINE"){
                matrix_pulse_voltage[pulse_voltage_count][0] = 2;   // 2 represents sine voltage source
                matrix_pulse_voltage[pulse_voltage_count][1] = num; // represents the number of the voltage
                matrix_pulse_voltage[pulse_voltage_count][2] = (*iter).Vinitial;    // represents DC offset
                matrix_pulse_voltage[pulse_voltage_count][3] = (*iter).Von; // represents Amplitude
                matrix_pulse_voltage[pulse_voltage_count][4] = (*iter).Ton; // represents Frequency
                matrix_pulse_voltage[pulse_voltage_count][5] = 2*pi*(*iter).Tperiod/360;     // represents Phase shift
                matrix_pulse_voltage[pulse_voltage_count][7] = round(1/((*iter).Ton * 0.0000001));
                pulse_voltage_count ++;
            }
            else {
                matrixB[node_number + num - 1] = (*iter).value;
            }
        }
        else if ((*iter).name[0] == 'I'){
            matrixB[(*iter).node1 - 1] = matrixB[(*iter).node1 - 1] - (*iter).value;
            matrixB[(*iter).node2 - 1] = matrixB[(*iter).node2 - 1] + (*iter).value;
        }
        else if ((*iter).name[0] == 'C'){
            matrix_nonlinear_components[nonlinear_components_count][0] = 1; // 1 represents capacitor
            matrix_nonlinear_components[nonlinear_components_count][1] = (*iter).value;
            matrix_nonlinear_components[nonlinear_components_count][2] = (*iter).node1;
            matrix_nonlinear_components[nonlinear_components_count][3] = (*iter).node2;
            nonlinear_components_count ++;

            if ((*iter).node1 == 0 || (*iter).node2 == 0){
                int num = (*iter).node1 == 0 ? (*iter).node2 : (*iter).node1;
                matrixA[num-1][num-1] = matrixA[num-1][num-1] + 2*(*iter).value/time_h;
            }
            else{
                matrixA[(*iter).node1 - 1][(*iter).node2 - 1] = matrixA[(*iter).node1 - 1][(*iter).node2 - 1] - 2*(*iter).value/time_h;
                matrixA[(*iter).node2 - 1][(*iter).node1 - 1] = matrixA[(*iter).node2 - 1][(*iter).node1 - 1] - 2*(*iter).value/time_h;
                matrixA[(*iter).node1 - 1][(*iter).node1 - 1] = matrixA[(*iter).node1 - 1][(*iter).node1 - 1] + 2*(*iter).value/time_h;
                matrixA[(*iter).node2 - 1][(*iter).node2 - 1] = matrixA[(*iter).node2 - 1][(*iter).node2 - 1] + 2*(*iter).value/time_h;
            }
        }
        else if ((*iter).name[0] == 'L'){
            matrix_nonlinear_components[nonlinear_components_count][0] = 2; // 2 represents inductor
            matrix_nonlinear_components[nonlinear_components_count][1] = (*iter).value;
            matrix_nonlinear_components[nonlinear_components_count][2] = (*iter).node1;
            matrix_nonlinear_components[nonlinear_components_count][3] = (*iter).node2;
            nonlinear_components_count ++;

            if ((*iter).node1 == 0 || (*iter).node2 == 0){
                int num = (*iter).node1 == 0 ? (*iter).node2 : (*iter).node1;
                matrixA[num-1][num-1] = matrixA[num-1][num-1] + time_h/(2*(*iter).value);
            }
            else{
                matrixA[(*iter).node1 - 1][(*iter).node2 - 1] = matrixA[(*iter).node1 - 1][(*iter).node2 - 1] - time_h/(2*(*iter).value);
                matrixA[(*iter).node2 - 1][(*iter).node1 - 1] = matrixA[(*iter).node2 - 1][(*iter).node1 - 1] - time_h/(2*(*iter).value);
                matrixA[(*iter).node1 - 1][(*iter).node1 - 1] = matrixA[(*iter).node1 - 1][(*iter).node1 - 1] + time_h/(2*(*iter).value);
                matrixA[(*iter).node2 - 1][(*iter).node2 - 1] = matrixA[(*iter).node2 - 1][(*iter).node2 - 1] + time_h/(2*(*iter).value);
            }
        }
        else if ((*iter).name[0] == 'E'){
            string name = (*iter).name;
            name.erase(name.begin());
            int num = stoi(name);
            if ((*iter).node1 != 0){
                matrixA[(*iter).node1 - 1][node_number + independent_voltage_source_number + num - 1] ++;
                matrixA[node_number + independent_voltage_source_number + num - 1][(*iter).node1 - 1] ++;
            }
            if ((*iter).node2 != 0){
                matrixA[(*iter).node2 - 1][node_number + independent_voltage_source_number + num - 1] --;
                matrixA[node_number + independent_voltage_source_number + num - 1][(*iter).node2 - 1] --;
            }
            if ((*iter).aux_node1 != 0){
                matrixA[node_number + independent_voltage_source_number + num - 1][(*iter).aux_node1 - 1] = matrixA[node_number + independent_voltage_source_number + num - 1][(*iter).aux_node1 - 1] -(*iter).value;
            }
            if ((*iter).aux_node2 != 0){
                matrixA[node_number + independent_voltage_source_number + num - 1][(*iter).aux_node2 - 1] = matrixA[node_number + independent_voltage_source_number + num - 1][(*iter).aux_node2 - 1] + (*iter).value;
            }
        }
        else if ((*iter).name[0] == 'G'){
            if ((*iter).node1 != 0){
                if ((*iter).aux_node1 != 0){
                    matrixA[(*iter).node1 - 1][(*iter).aux_node1 - 1] = matrixA[(*iter).node1 - 1][(*iter).aux_node1 - 1] + (*iter).value;
                }
                if ((*iter).aux_node2 != 0){
                    matrixA[(*iter).node1 - 1][(*iter).aux_node2 - 1] = matrixA[(*iter).node1 - 1][(*iter).aux_node2 - 1] - (*iter).value;
                }
            }
            if ((*iter).node2 != 0){
                if ((*iter).aux_node1 != 0){
                    matrixA[(*iter).node2 - 1][(*iter).aux_node1 - 1] = matrixA[(*iter).node2 - 1][(*iter).aux_node1 - 1] - (*iter).value;
                }
                if ((*iter).aux_node2 != 0){
                    matrixA[(*iter).node2 - 1][(*iter).aux_node2 - 1] = matrixA[(*iter).node2 - 1][(*iter).aux_node2 - 1] + (*iter).value;
                }
            }
        }
        else if ((*iter).name[0] == 'D'){
            matrix_diode[diode_count][0] = (*iter).node1;
            matrix_diode[diode_count][1] = (*iter).node2;
            diode_count ++;
        }
        else if ((*iter).name[0] == 'M'){
            matrix_mosfet[mosfet_count][0] = (*iter).node1; //Drain
            matrix_mosfet[mosfet_count][1] = (*iter).node2; //Gate
            matrix_mosfet[mosfet_count][2] = (*iter).aux_node1; //Source
            mosfet_count ++;
        }
        iter ++;
    }
    
    Trapezoidal_analysis(matrixA, matrixB, matrix_nonlinear_components, matrix_pulse_voltage, 
    matrix_switch, matrix_diode, matrix_mosfet, matrix_size, node_number, time_h, nonlinear_components, pulse_voltage_source_number, 
    switch_number, diode_number, mosfet_number, total_steps);

    int Vcount = 1;
    int Icount = 1;
    for (int i = 0; i < matrix_size; i++){
        if (Vcount <= node_number){
            outFile << 'V' << Vcount << " : " << matrixB[i] << 'V' << endl;
            Vcount ++;
        }
        else{
            outFile << 'I' << Icount << " : " << matrixB[i] << 'A' << endl;
            Icount ++;
        }
    }
    
    outFile.close();
    inFile.close();
    return 0;
}
