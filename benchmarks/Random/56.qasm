OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[56];
creg c[56];

ccx qubits[7], qubits[43], qubits[37];
ccx qubits[34], qubits[33], qubits[10];
cx qubits[5], qubits[19];
cz qubits[30], qubits[21];
y qubits[0];
t qubits[33];
z qubits[28];
s qubits[17];
s qubits[10];
y qubits[21];
y qubits[25];
ry(pi/2) qubits[6];
t qubits[22];
y qubits[37];
s qubits[1];
rx(pi/2) qubits[20];
s qubits[22];
s qubits[30];
y qubits[55];
s qubits[14];
x qubits[2];
z qubits[29];
cx qubits[32], qubits[31];
z qubits[42];
z qubits[40];
h qubits[11];
h qubits[15];
cx qubits[42], qubits[12];
h qubits[29];
s qubits[27];
s qubits[24];
y qubits[2];
y qubits[22];
cx qubits[18], qubits[20];
cz qubits[4], qubits[36];
cz qubits[19], qubits[12];
cx qubits[22], qubits[38];
cx qubits[46], qubits[48];
x qubits[11];
ry(pi/2) qubits[16];
t qubits[20];
ccx qubits[51], qubits[33], qubits[54];
s qubits[42];
cz qubits[31], qubits[8];
ccx qubits[16], qubits[26], qubits[2];
z qubits[15];
t qubits[24];
cz qubits[34], qubits[37];
cz qubits[16], qubits[21];
t qubits[8];
h qubits[41];
cz qubits[31], qubits[23];
y qubits[2];
cz qubits[37], qubits[40];
z qubits[7];
y qubits[45];
ccx qubits[13], qubits[6], qubits[26];
ccx qubits[31], qubits[41], qubits[21];
rx(pi/2) qubits[3];
h qubits[21];
cx qubits[0], qubits[42];
h qubits[48];
t qubits[19];
z qubits[17];
z qubits[52];
y qubits[28];
rx(pi/2) qubits[4];
y qubits[25];
ry(pi/2) qubits[32];
ccx qubits[17], qubits[38], qubits[36];
h qubits[54];
s qubits[22];
ry(pi/2) qubits[9];
ry(pi/2) qubits[2];
y qubits[46];
cx qubits[46], qubits[38];
h qubits[10];
s qubits[47];
ry(pi/2) qubits[11];
t qubits[9];
cz qubits[38], qubits[52];
ccx qubits[42], qubits[28], qubits[48];
ry(pi/2) qubits[51];
ccx qubits[45], qubits[49], qubits[34];
z qubits[30];
cx qubits[41], qubits[33];
ry(pi/2) qubits[31];
cx qubits[30], qubits[14];
cz qubits[40], qubits[29];
z qubits[4];
x qubits[29];
rx(pi/2) qubits[54];
s qubits[49];
ry(pi/2) qubits[54];
h qubits[29];
z qubits[17];
y qubits[55];
x qubits[20];
y qubits[40];
x qubits[20];
z qubits[30];
x qubits[47];
rx(pi/2) qubits[49];
ry(pi/2) qubits[28];
y qubits[5];
cz qubits[47], qubits[34];
z qubits[45];
ry(pi/2) qubits[8];
z qubits[11];
s qubits[23];
s qubits[31];
s qubits[36];
y qubits[13];
h qubits[49];
t qubits[35];
rx(pi/2) qubits[19];
ccx qubits[42], qubits[7], qubits[24];
cz qubits[20], qubits[30];
rx(pi/2) qubits[37];
h qubits[43];
s qubits[50];
rx(pi/2) qubits[3];
cz qubits[28], qubits[17];
x qubits[50];
cx qubits[45], qubits[30];
ry(pi/2) qubits[3];
s qubits[35];
z qubits[8];
s qubits[50];
y qubits[40];
x qubits[47];
s qubits[22];
rx(pi/2) qubits[46];
s qubits[11];
cz qubits[38], qubits[45];
z qubits[26];
h qubits[7];
rx(pi/2) qubits[30];
cz qubits[4], qubits[19];
y qubits[29];
z qubits[45];
cz qubits[38], qubits[37];
z qubits[16];
cx qubits[46], qubits[2];
x qubits[11];
cz qubits[41], qubits[51];
s qubits[18];
cz qubits[25], qubits[37];
cx qubits[3], qubits[44];
h qubits[17];
s qubits[3];
h qubits[28];
s qubits[27];
cz qubits[25], qubits[49];
s qubits[32];
t qubits[27];
ry(pi/2) qubits[12];
z qubits[33];
ry(pi/2) qubits[32];
t qubits[16];
cz qubits[21], qubits[34];
s qubits[2];
x qubits[25];
h qubits[22];
x qubits[27];
t qubits[7];
ccx qubits[42], qubits[19], qubits[41];
ccx qubits[52], qubits[8], qubits[44];
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
