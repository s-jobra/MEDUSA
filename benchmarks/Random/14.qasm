OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[14];
creg c[14];

z qubits[12];
ry(pi/2) qubits[2];
x qubits[1];
x qubits[7];
s qubits[11];
rx(pi/2) qubits[1];
t qubits[0];
ccx qubits[12], qubits[9], qubits[1];
rx(pi/2) qubits[3];
s qubits[6];
y qubits[13];
h qubits[6];
y qubits[4];
t qubits[6];
cx qubits[12], qubits[10];
ry(pi/2) qubits[1];
h qubits[3];
y qubits[6];
z qubits[10];
ccx qubits[13], qubits[8], qubits[2];
cz qubits[3], qubits[11];
rx(pi/2) qubits[11];
s qubits[5];
s qubits[12];
cx qubits[13], qubits[4];
ry(pi/2) qubits[1];
cx qubits[1], qubits[7];
x qubits[10];
t qubits[3];
ry(pi/2) qubits[4];
y qubits[12];
ccx qubits[3], qubits[6], qubits[0];
ry(pi/2) qubits[8];
z qubits[13];
s qubits[3];
ry(pi/2) qubits[1];
h qubits[11];
ccx qubits[10], qubits[1], qubits[5];
s qubits[5];
s qubits[5];
cz qubits[3], qubits[4];
cz qubits[9], qubits[6];
measure qubits[0] -> c[0];
measure qubits[1] -> c[1];
measure qubits[2] -> c[2];
measure qubits[3] -> c[3];
measure qubits[4] -> c[4];
measure qubits[5] -> c[5];
measure qubits[6] -> c[6];
measure qubits[7] -> c[7];
measure qubits[8] -> c[8];
measure qubits[9] -> c[9];
measure qubits[10] -> c[10];
measure qubits[11] -> c[11];
measure qubits[12] -> c[12];
measure qubits[13] -> c[13];