OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[7];
creg c[7];

cx qubits[4], qubits[5];
cx qubits[6], qubits[1];
cx qubits[6], qubits[2];
ccx qubits[4], qubits[5], qubits[6];
cx qubits[6], qubits[4];
cx qubits[6], qubits[5];
cx qubits[3], qubits[2];
cx qubits[3], qubits[0];
cx qubits[3], qubits[1];
ccx qubits[2], qubits[5], qubits[3];
ccx qubits[4], qubits[6], qubits[2];
ccx qubits[2], qubits[5], qubits[3];
cx qubits[3], qubits[1];
ccx qubits[5], qubits[6], qubits[4];
cx qubits[3], qubits[0];
cx qubits[3], qubits[5];
ccx qubits[4], qubits[6], qubits[3];
cx qubits[1], qubits[4];
cx qubits[2], qubits[1];
cx qubits[3], qubits[6];
cx qubits[2], qubits[5];
cx qubits[1], qubits[3];
cx qubits[3], qubits[0];
cx qubits[0], qubits[3];
cx qubits[0], qubits[2];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
measure qubits[4] -> c[4];
measure qubits[5] -> c[5];
measure qubits[6] -> c[6];