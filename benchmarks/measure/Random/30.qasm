OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[30];
creg c[30];

cz qubits[1], qubits[7];
ry(pi/2) qubits[29];
cz qubits[19], qubits[29];
cx qubits[7], qubits[3];
ccx qubits[5], qubits[2], qubits[18];
z qubits[10];
cz qubits[4], qubits[19];
z qubits[16];
cz qubits[29], qubits[6];
ccx qubits[18], qubits[28], qubits[27];
x qubits[7];
x qubits[2];
x qubits[19];
rx(pi/2) qubits[23];
rx(pi/2) qubits[1];
x qubits[17];
s qubits[11];
z qubits[29];
z qubits[10];
x qubits[18];
cz qubits[13], qubits[14];
y qubits[0];
ccx qubits[29], qubits[4], qubits[24];
ry(pi/2) qubits[1];
rx(pi/2) qubits[19];
cx qubits[14], qubits[1];
cx qubits[15], qubits[17];
y qubits[16];
ry(pi/2) qubits[21];
y qubits[21];
ccx qubits[3], qubits[5], qubits[7];
cx qubits[23], qubits[13];
ccx qubits[0], qubits[27], qubits[24];
z qubits[26];
t qubits[21];
h qubits[29];
z qubits[27];
y qubits[17];
h qubits[6];
y qubits[7];
rx(pi/2) qubits[2];
s qubits[1];
ccx qubits[23], qubits[28], qubits[2];
x qubits[5];
s qubits[22];
t qubits[23];
cx qubits[7], qubits[17];
ccx qubits[26], qubits[15], qubits[25];
x qubits[6];
cx qubits[1], qubits[10];
z qubits[21];
cz qubits[9], qubits[21];
s qubits[11];
cz qubits[19], qubits[17];
t qubits[9];
t qubits[22];
z qubits[2];
cz qubits[16], qubits[25];
rx(pi/2) qubits[15];
s qubits[23];
h qubits[19];
cz qubits[7], qubits[17];
rx(pi/2) qubits[8];
h qubits[23];
x qubits[5];
ry(pi/2) qubits[13];
h qubits[27];
y qubits[26];
ry(pi/2) qubits[29];
z qubits[18];
x qubits[3];
ry(pi/2) qubits[19];
cz qubits[16], qubits[1];
h qubits[21];
t qubits[0];
h qubits[5];
rx(pi/2) qubits[20];
cx qubits[16], qubits[9];
z qubits[0];
rx(pi/2) qubits[1];
h qubits[22];
t qubits[19];
t qubits[6];
ccx qubits[16], qubits[24], qubits[23];
cx qubits[15], qubits[4];
s qubits[24];
cx qubits[28], qubits[15];
x qubits[29];
ry(pi/2) qubits[26];
ry(pi/2) qubits[29];
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
measure qubits[19] -> c[19];
measure qubits[20] -> c[20];
measure qubits[21] -> c[21];
measure qubits[22] -> c[22];
measure qubits[23] -> c[23];
measure qubits[24] -> c[24];
measure qubits[25] -> c[25];
measure qubits[26] -> c[26];
measure qubits[27] -> c[27];
measure qubits[28] -> c[28];
measure qubits[29] -> c[29];