# SPICE-like C++ Analog Circuit Simulator

### Author: Po Yu Chang

## Overview
This is a **SPICE-like Analog Circuit Simulator** designed for **Transient (time-domain) analysis**. It supports basic analog components and devices, with a working frequency limit of **1 MHz**. Please ensure you do not exceed this frequency during simulations.

## Supported Devices
The simulator supports the following device models:
- **Resistor (R):** `R <node1> <node2> <value>`
- **Capacitor (C):** `C <node1> <node2> <value>`
- **Inductor (L):** `L <node1> <node2> <value>`
- **Independent DC voltage source (V):** `V <node1> <node2> <value>`
- **Independent DC current source (I):** `I <node1> <node2> <value>`
- **Voltage controlled voltage source (E):** `E <node1> <node2> <ref_node1> <ref_node2> <amp_value>`
- **Voltage controlled current source (G):** `G <node1> <node2> <ref_node1> <ref_node2> <amp_value>`
- **Voltage-dependent switch (S):** `S <node1> <node2> <ref_node1> <ref_node2> <switch name>`
- **Diode (D):** `D <node1> <node2>` (ideal diode: forward bias = short circuit, reverse bias = open circuit)

### Voltage Sources
- **Pulse voltage source:** `V <node1> <node2> PULSE <Vinitial> <Von> <Ton> <Tperiod>`
- **Sinusoidal voltage source:** `V <node1> <node2> SINE <DC offset> <Amplitude> <Frequency> <Phase shift>`

### Switch Model
- **Switch:** `.model <switch name> sw <Ron> <Roff> <Vt>`

### In Development
- **NMOS Transistor (M):** `M <drain> <gate> <source> <body>` (connect body to source). The MOSFET model includes the following parameters: 
  - L = 180nm 
  - W = 360nm 
  - μn = 450 cm²/Vs 
  - Cox = 8.6 fF/μm² 
  - Vt = 0.48V 
  - VA' = 5V/μm

## Supported Analysis
- **Transient Analysis:** `.tran <time>`

## Simulation Process
1. **Modified Nodal Analysis (MNA)**
2. **Gaussian Elimination**
3. **Trapezoidal Method** for solving nonlinear circuits (a combination of Forward and Backward Euler methods)
4. **Python for graphing** the time-domain simulation results.

## Netlist Writing Rules
1. Always begin with `*****` on the first line.
2. Use power-of-ten notation: T(12), G(9), Meg(6), k(3), m(-3), u(-6), n(-9), p(-12), f(-15).
3. Nodes should be typed as numbers.
   - Example: `R1 1 0 2k`
4. Components should be labeled sequentially, starting from 1.
   - Example: `R1 R2 R3 V1 V2 V3`
5. For `.tran`, specify the time without including "seconds".
   - Example: `.tran 100m`
6. For controlled sources, omit descriptive words in parameters.
   - Example: `E 3 2 1 0 3.3`
7. For pulse/sine voltage sources, omit brackets.
   - Example: `V2 1 0 PULSE 0 2 1m 2m`
8. For switch models, omit parameters such as Von, Voff, or Vt.
   - Example: `.model SW sw 0.5 1G 1`
9. Pulse voltage sources do not support `Tdelay`, `Trise`, or `Tfall`.
10. The `.model` line must come after declaring the switch in the netlist.

## Running the Simulator
To run the simulator:
1. Compile the code:
   ```bash
   g++ spice_simulation.cpp Guassian_elimination.cpp Trapezoidal_analysis.cpp -o spice_simulation.exe
2. Run the code:
   ```bash
   ./spice_simulation.exe
3. Show the result:
   ```bash
   python graph.py

## Result
1. out.txt (Displays every node voltage and current through each voltage source)
2. Time-domain simulation graph displayed after running graph.py
3. If nothing is shown on either out.txt or python graph :

    a. Check netlist adherence to rules

    b. Ensure circuit validity

    c. Maybe there are some mistakes in my code XD

## Example Netlists

### 1. RLC Resonant Circuit
This example demonstrates a simple RLC resonant circuit with a DC voltage source, resistor, capacitor, and inductor.

```plaintext
******
V1 1 0 10          ; DC voltage source of 10V
R1 2 1 5           ; Resistor with 5 ohms between nodes 2 and 1
C1 3 0 20u         ; Capacitor with 20μF between nodes 3 and 0
L1 2 3 50m         ; Inductor with 50mH between nodes 2 and 3
.tran 300m         ; Transient analysis for 300ms
.end               ; End of the netlist
```

### 2. RC Filter
This example showcases a pulse-driven RC filter, consisting of a resistor and capacitor with a pulse voltage source.

```plaintext
*****
V1 1 0 PULSE 0 5 10u 20u   ; Pulse voltage source with 0V initial, 5V on, 10µs delay, 20µs period
R1 0 2 1k                  ; Resistor with 1kΩ between nodes 0 and 2
C1 2 1 10n                 ; Capacitor with 10nF between nodes 2 and 1
.tran 100u                 ; Transient analysis for 100µs
.end                       ; End of the netlist
```
