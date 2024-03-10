OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[3];
creg c[3];

h qubits[0];
h qubits[1];
h qubits[2];
z qubits[2];
cx qubits[0], qubits[2];
h qubits[0];
h qubits[1];
h qubits[2];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
