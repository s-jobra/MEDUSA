OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[5];
creg c[5];

cx qubits[4], qubits[0];
x qubits[4];
ccx qubits[2], qubits[1], qubits[0];
cx qubits[0], qubits[4];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
measure qubits[4] -> c[4];
