OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[74];
creg c[74];

s qubits[52];
ry(pi/2) qubits[60];
ccx qubits[3], qubits[54], qubits[68];
rx(pi/2) qubits[40];
x qubits[31];
ry(pi/2) qubits[65];
cz qubits[38], qubits[45];
x qubits[73];
rx(pi/2) qubits[73];
h qubits[42];
cx qubits[39], qubits[6];
z qubits[24];
cx qubits[50], qubits[6];
x qubits[6];
ccx qubits[1], qubits[18], qubits[28];
cx qubits[12], qubits[36];
ccx qubits[17], qubits[45], qubits[33];
t qubits[58];
cx qubits[31], qubits[35];
ry(pi/2) qubits[46];
cz qubits[57], qubits[14];
ry(pi/2) qubits[0];
ry(pi/2) qubits[31];
x qubits[49];
x qubits[30];
t qubits[66];
y qubits[43];
h qubits[39];
ccx qubits[1], qubits[53], qubits[18];
ry(pi/2) qubits[65];
ccx qubits[61], qubits[40], qubits[13];
rx(pi/2) qubits[67];
ry(pi/2) qubits[13];
x qubits[52];
ccx qubits[50], qubits[73], qubits[53];
z qubits[58];
ry(pi/2) qubits[67];
h qubits[20];
ry(pi/2) qubits[61];
rx(pi/2) qubits[16];
x qubits[17];
h qubits[58];
s qubits[17];
x qubits[59];
x qubits[31];
s qubits[29];
x qubits[73];
s qubits[69];
cx qubits[34], qubits[23];
h qubits[13];
h qubits[24];
y qubits[72];
x qubits[21];
t qubits[40];
h qubits[29];
t qubits[53];
y qubits[64];
s qubits[50];
s qubits[37];
s qubits[39];
rx(pi/2) qubits[41];
t qubits[0];
ccx qubits[29], qubits[41], qubits[58];
cz qubits[7], qubits[71];
y qubits[31];
h qubits[4];
t qubits[49];
y qubits[72];
h qubits[25];
z qubits[43];
y qubits[46];
cx qubits[35], qubits[54];
cx qubits[25], qubits[72];
h qubits[66];
ccx qubits[58], qubits[68], qubits[40];
z qubits[31];
z qubits[32];
y qubits[55];
rx(pi/2) qubits[4];
ccx qubits[0], qubits[31], qubits[33];
cz qubits[43], qubits[58];
cz qubits[12], qubits[20];
t qubits[10];
h qubits[60];
h qubits[58];
ry(pi/2) qubits[8];
y qubits[69];
ccx qubits[23], qubits[35], qubits[70];
y qubits[32];
cz qubits[60], qubits[13];
z qubits[43];
x qubits[27];
t qubits[9];
ry(pi/2) qubits[21];
cz qubits[60], qubits[33];
cx qubits[14], qubits[44];
x qubits[0];
s qubits[62];
rx(pi/2) qubits[2];
y qubits[32];
t qubits[17];
cz qubits[44], qubits[71];
x qubits[29];
z qubits[68];
y qubits[57];
y qubits[9];
cx qubits[32], qubits[14];
ry(pi/2) qubits[55];
cz qubits[16], qubits[25];
ry(pi/2) qubits[38];
s qubits[66];
ry(pi/2) qubits[17];
cz qubits[35], qubits[11];
cz qubits[8], qubits[29];
ry(pi/2) qubits[30];
ry(pi/2) qubits[65];
y qubits[40];
rx(pi/2) qubits[19];
y qubits[8];
x qubits[59];
t qubits[21];
z qubits[21];
y qubits[58];
z qubits[13];
cx qubits[54], qubits[57];
z qubits[49];
ccx qubits[46], qubits[60], qubits[8];
h qubits[15];
y qubits[41];
h qubits[62];
t qubits[38];
x qubits[2];
ccx qubits[12], qubits[9], qubits[25];
ry(pi/2) qubits[8];
cz qubits[29], qubits[7];
x qubits[54];
s qubits[30];
z qubits[34];
cz qubits[9], qubits[3];
h qubits[70];
rx(pi/2) qubits[69];
s qubits[60];
ccx qubits[9], qubits[27], qubits[20];
t qubits[53];
t qubits[66];
y qubits[62];
x qubits[73];
rx(pi/2) qubits[62];
z qubits[56];
ry(pi/2) qubits[9];
t qubits[48];
ccx qubits[66], qubits[16], qubits[31];
cx qubits[30], qubits[5];
s qubits[26];
y qubits[45];
s qubits[56];
rx(pi/2) qubits[39];
y qubits[29];
cz qubits[0], qubits[72];
ry(pi/2) qubits[69];
y qubits[24];
cx qubits[26], qubits[58];
s qubits[13];
cz qubits[4], qubits[34];
ry(pi/2) qubits[72];
cx qubits[4], qubits[29];
t qubits[63];
cx qubits[21], qubits[13];
y qubits[3];
ry(pi/2) qubits[39];
x qubits[41];
h qubits[63];
s qubits[10];
rx(pi/2) qubits[37];
y qubits[71];
cx qubits[70], qubits[27];
ry(pi/2) qubits[12];
cz qubits[66], qubits[73];
cz qubits[14], qubits[4];
y qubits[62];
s qubits[48];
z qubits[58];
cx qubits[64], qubits[48];
z qubits[53];
x qubits[48];
s qubits[32];
z qubits[56];
cx qubits[70], qubits[31];
h qubits[66];
s qubits[24];
h qubits[25];
h qubits[56];
cz qubits[9], qubits[38];
x qubits[71];
ccx qubits[53], qubits[59], qubits[24];
t qubits[49];
h qubits[71];
ry(pi/2) qubits[13];
cz qubits[49], qubits[45];
cz qubits[10], qubits[19];
ccx qubits[19], qubits[11], qubits[25];
x qubits[36];
t qubits[7];
h qubits[64];
ccx qubits[40], qubits[28], qubits[24];
ccx qubits[15], qubits[3], qubits[0];
h qubits[9];
cz qubits[15], qubits[59];
h qubits[28];
x qubits[32];
x qubits[46];
ccx qubits[49], qubits[27], qubits[40];
ccx qubits[53], qubits[22], qubits[52];
h qubits[8];
x qubits[29];
h qubits[27];
rx(pi/2) qubits[29];
t qubits[28];
z qubits[21];
rx(pi/2) qubits[72];
s qubits[61];
h qubits[10];
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
measure qubits[51] -> c[51];
measure qubits[52] -> c[52];
measure qubits[53] -> c[53];
measure qubits[54] -> c[54];
measure qubits[55] -> c[55];
measure qubits[56] -> c[56];
measure qubits[57] -> c[57];
measure qubits[58] -> c[58];
measure qubits[59] -> c[59];
measure qubits[60] -> c[60];
measure qubits[61] -> c[61];
measure qubits[62] -> c[62];
measure qubits[63] -> c[63];
measure qubits[64] -> c[64];
measure qubits[65] -> c[65];
measure qubits[66] -> c[66];
measure qubits[67] -> c[67];
measure qubits[68] -> c[68];
measure qubits[69] -> c[69];
measure qubits[70] -> c[70];
measure qubits[71] -> c[71];
measure qubits[72] -> c[72];
measure qubits[73] -> c[73];