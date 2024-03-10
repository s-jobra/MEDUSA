OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[4];
creg c[4];

ccx qubits[3], qubits[2], qubits[0];
cx qubits[2], qubits[3];
ccx qubits[3], qubits[2], qubits[1];
ccx qubits[2], qubits[0], qubits[3];
cx qubits[3], qubits[2];
x qubits[3];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
