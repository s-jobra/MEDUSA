OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[19];
creg c[19];

ry(pi/2) qubits[12];
h qubits[4];
x qubits[15];
s qubits[11];
ry(pi/2) qubits[11];
x qubits[9];
cz qubits[1], qubits[2];
ry(pi/2) qubits[16];
z qubits[13];
rx(pi/2) qubits[8];
t qubits[12];
ry(pi/2) qubits[4];
s qubits[0];
rx(pi/2) qubits[0];
x qubits[16];
y qubits[6];
cx qubits[5], qubits[3];
ry(pi/2) qubits[14];
y qubits[10];
ry(pi/2) qubits[8];
rx(pi/2) qubits[10];
s qubits[8];
ry(pi/2) qubits[15];
s qubits[15];
t qubits[16];
ccx qubits[9], qubits[17], qubits[2];
cx qubits[18], qubits[0];
rx(pi/2) qubits[15];
ry(pi/2) qubits[10];
cz qubits[6], qubits[15];
h qubits[5];
h qubits[5];
h qubits[1];
t qubits[1];
h qubits[0];
ccx qubits[16], qubits[15], qubits[9];
x qubits[14];
ccx qubits[16], qubits[4], qubits[1];
cz qubits[14], qubits[0];
s qubits[14];
rx(pi/2) qubits[16];
s qubits[13];
rx(pi/2) qubits[2];
rx(pi/2) qubits[8];
z qubits[3];
x qubits[10];
y qubits[3];
z qubits[14];
t qubits[13];
y qubits[18];
cx qubits[3], qubits[18];
t qubits[4];
t qubits[11];
rx(pi/2) qubits[16];
cx qubits[11], qubits[10];
ry(pi/2) qubits[16];
cz qubits[8], qubits[15];
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
measure qubits[14] -> c[14];
measure qubits[15] -> c[15];
measure qubits[16] -> c[16];
measure qubits[17] -> c[17];
measure qubits[18] -> c[18];