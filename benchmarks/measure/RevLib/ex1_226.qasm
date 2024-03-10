OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[6];
creg c[6];

cx qubits[3], qubits[0];
cx qubits[1], qubits[0];
cx qubits[5], qubits[0];
x qubits[2];
cx qubits[2], qubits[0];
x qubits[4];
cx qubits[4], qubits[0];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
measure qubits[4] -> c[4];
measure qubits[5] -> c[5];
