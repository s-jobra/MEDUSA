OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[51];
creg c[51];

cx qubits[9], qubits[14];
s qubits[17];
x qubits[7];
ccx qubits[19], qubits[3], qubits[5];
ccx qubits[9], qubits[15], qubits[33];
h qubits[17];
s qubits[44];
rx(pi/2) qubits[44];
t qubits[36];
cz qubits[29], qubits[25];
z qubits[26];
cx qubits[15], qubits[33];
h qubits[49];
ccx qubits[41], qubits[16], qubits[42];
s qubits[19];
ccx qubits[0], qubits[50], qubits[15];
t qubits[39];
z qubits[20];
ry(pi/2) qubits[39];
s qubits[29];
cz qubits[46], qubits[14];
ry(pi/2) qubits[50];
t qubits[16];
y qubits[32];
cz qubits[6], qubits[5];
rx(pi/2) qubits[27];
x qubits[45];
t qubits[18];
x qubits[28];
x qubits[38];
cz qubits[28], qubits[0];
x qubits[22];
z qubits[27];
z qubits[8];
h qubits[6];
s qubits[0];
ry(pi/2) qubits[36];
t qubits[29];
ccx qubits[22], qubits[9], qubits[1];
h qubits[43];
x qubits[3];
t qubits[34];
ccx qubits[28], qubits[21], qubits[22];
cx qubits[46], qubits[10];
cz qubits[0], qubits[12];
cz qubits[8], qubits[45];
ccx qubits[5], qubits[19], qubits[47];
s qubits[10];
ccx qubits[6], qubits[8], qubits[28];
x qubits[44];
ry(pi/2) qubits[3];
h qubits[30];
ccx qubits[26], qubits[32], qubits[33];
cx qubits[37], qubits[28];
s qubits[39];
cz qubits[45], qubits[50];
y qubits[13];
rx(pi/2) qubits[41];
x qubits[13];
ccx qubits[17], qubits[13], qubits[36];
h qubits[21];
ry(pi/2) qubits[5];
cz qubits[33], qubits[8];
y qubits[12];
z qubits[11];
cz qubits[1], qubits[7];
ccx qubits[4], qubits[14], qubits[44];
h qubits[8];
z qubits[42];
s qubits[9];
rx(pi/2) qubits[28];
s qubits[11];
z qubits[10];
s qubits[16];
y qubits[10];
cz qubits[20], qubits[43];
y qubits[31];
h qubits[48];
y qubits[24];
t qubits[23];
ry(pi/2) qubits[27];
cz qubits[47], qubits[8];
cz qubits[13], qubits[24];
x qubits[23];
cx qubits[18], qubits[20];
x qubits[37];
cz qubits[12], qubits[28];
cx qubits[22], qubits[23];
cx qubits[15], qubits[27];
ry(pi/2) qubits[20];
cx qubits[0], qubits[18];
z qubits[23];
ry(pi/2) qubits[49];
x qubits[17];
ccx qubits[5], qubits[30], qubits[29];
cx qubits[27], qubits[34];
x qubits[47];
h qubits[50];
cx qubits[37], qubits[1];
s qubits[9];
z qubits[15];
rx(pi/2) qubits[26];
rx(pi/2) qubits[18];
y qubits[24];
t qubits[45];
h qubits[31];
y qubits[6];
t qubits[48];
y qubits[27];
rx(pi/2) qubits[31];
s qubits[35];
ry(pi/2) qubits[25];
z qubits[18];
ccx qubits[21], qubits[19], qubits[10];
y qubits[18];
z qubits[28];
x qubits[49];
y qubits[17];
ry(pi/2) qubits[5];
h qubits[43];
h qubits[29];
ccx qubits[7], qubits[1], qubits[34];
cz qubits[2], qubits[40];
x qubits[11];
z qubits[12];
t qubits[25];
y qubits[42];
y qubits[22];
ccx qubits[37], qubits[21], qubits[23];
ry(pi/2) qubits[44];
h qubits[22];
rx(pi/2) qubits[22];
cz qubits[32], qubits[38];
y qubits[39];
cz qubits[39], qubits[47];
h qubits[28];
s qubits[26];
rx(pi/2) qubits[21];
rx(pi/2) qubits[35];
t qubits[37];
z qubits[29];
h qubits[41];
z qubits[4];
h qubits[17];
ccx qubits[17], qubits[6], qubits[18];
cx qubits[12], qubits[24];
t qubits[17];
ccx qubits[1], qubits[31], qubits[9];
s qubits[8];
t qubits[42];
ry(pi/2) qubits[38];
h qubits[3];
h qubits[50];
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
measure qubits[30] -> c[30];
measure qubits[31] -> c[31];
measure qubits[32] -> c[32];
measure qubits[33] -> c[33];
measure qubits[34] -> c[34];
measure qubits[35] -> c[35];
measure qubits[36] -> c[36];
measure qubits[37] -> c[37];
measure qubits[38] -> c[38];
measure qubits[39] -> c[39];
measure qubits[40] -> c[40];
measure qubits[41] -> c[41];
measure qubits[42] -> c[42];
measure qubits[43] -> c[43];
measure qubits[44] -> c[44];
measure qubits[45] -> c[45];
measure qubits[46] -> c[46];
measure qubits[47] -> c[47];
measure qubits[48] -> c[48];
measure qubits[49] -> c[49];
measure qubits[50] -> c[50];