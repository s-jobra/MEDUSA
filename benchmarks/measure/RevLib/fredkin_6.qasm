OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[3];
creg c[3];

ccx qubits[0], qubits[2], qubits[1];
ccx qubits[1], qubits[0], qubits[2];
ccx qubits[0], qubits[2], qubits[1];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
