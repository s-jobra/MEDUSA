OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[6];
creg c[6];

ccx qubits[0], qubits[1], qubits[2];
ccx qubits[3], qubits[2], qubits[4];
cx qubits[4], qubits[5];
ccx qubits[3], qubits[2], qubits[4];
ccx qubits[0], qubits[1], qubits[2];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
measure qubits[4] -> c[4];
measure qubits[5] -> c[5];
