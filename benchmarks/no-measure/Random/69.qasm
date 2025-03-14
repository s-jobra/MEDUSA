OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[69];

rx(pi/2) qubits[31];
x qubits[49];
cx qubits[67], qubits[4];
ccx qubits[28], qubits[39], qubits[57];
ry(pi/2) qubits[25];
rx(pi/2) qubits[21];
cz qubits[2], qubits[45];
h qubits[22];
ry(pi/2) qubits[43];
t qubits[22];
x qubits[9];
rx(pi/2) qubits[33];
ry(pi/2) qubits[15];
cz qubits[53], qubits[17];
cz qubits[5], qubits[4];
rx(pi/2) qubits[49];
z qubits[23];
z qubits[66];
cx qubits[16], qubits[17];
t qubits[65];
s qubits[31];
ccx qubits[12], qubits[14], qubits[23];
s qubits[36];
rx(pi/2) qubits[9];
z qubits[2];
ry(pi/2) qubits[39];
ccx qubits[38], qubits[28], qubits[4];
x qubits[3];
z qubits[63];
s qubits[61];
cz qubits[9], qubits[27];
cz qubits[26], qubits[42];
h qubits[17];
ry(pi/2) qubits[17];
h qubits[19];
cz qubits[0], qubits[26];
s qubits[9];
ccx qubits[50], qubits[23], qubits[61];
h qubits[32];
x qubits[33];
rx(pi/2) qubits[63];
h qubits[40];
cx qubits[35], qubits[26];
x qubits[62];
ccx qubits[53], qubits[6], qubits[36];
rx(pi/2) qubits[51];
s qubits[1];
cx qubits[35], qubits[2];
cx qubits[14], qubits[11];
ry(pi/2) qubits[36];
ry(pi/2) qubits[52];
z qubits[8];
z qubits[0];
cx qubits[8], qubits[42];
x qubits[54];
ry(pi/2) qubits[63];
x qubits[41];
x qubits[65];
s qubits[68];
t qubits[1];
t qubits[9];
cx qubits[29], qubits[11];
t qubits[15];
ry(pi/2) qubits[33];
cz qubits[39], qubits[57];
cz qubits[48], qubits[2];
cx qubits[4], qubits[51];
ccx qubits[1], qubits[7], qubits[11];
x qubits[19];
cz qubits[28], qubits[55];
z qubits[67];
y qubits[44];
s qubits[33];
cx qubits[22], qubits[15];
z qubits[37];
t qubits[35];
z qubits[20];
ccx qubits[8], qubits[39], qubits[66];
cx qubits[14], qubits[19];
rx(pi/2) qubits[56];
cx qubits[11], qubits[51];
s qubits[5];
y qubits[3];
y qubits[49];
x qubits[63];
z qubits[31];
cz qubits[64], qubits[17];
cz qubits[33], qubits[67];
rx(pi/2) qubits[23];
ccx qubits[55], qubits[3], qubits[68];
ccx qubits[40], qubits[13], qubits[2];
ry(pi/2) qubits[0];
t qubits[22];
cx qubits[56], qubits[28];
cz qubits[60], qubits[47];
ry(pi/2) qubits[58];
rx(pi/2) qubits[66];
ry(pi/2) qubits[64];
cx qubits[8], qubits[49];
x qubits[46];
rx(pi/2) qubits[20];
cx qubits[26], qubits[63];
h qubits[49];
z qubits[19];
rx(pi/2) qubits[6];
cx qubits[62], qubits[0];
h qubits[49];
ccx qubits[18], qubits[11], qubits[46];
ry(pi/2) qubits[40];
cx qubits[11], qubits[31];
cx qubits[44], qubits[11];
cx qubits[41], qubits[28];
y qubits[61];
z qubits[42];
ry(pi/2) qubits[0];
cx qubits[62], qubits[19];
s qubits[39];
z qubits[49];
cz qubits[23], qubits[29];
rx(pi/2) qubits[12];
cz qubits[15], qubits[26];
t qubits[4];
rx(pi/2) qubits[15];
x qubits[52];
z qubits[61];
cx qubits[25], qubits[37];
h qubits[60];
ry(pi/2) qubits[14];
cx qubits[43], qubits[7];
y qubits[30];
y qubits[32];
ry(pi/2) qubits[28];
ry(pi/2) qubits[40];
s qubits[10];
x qubits[53];
y qubits[35];
h qubits[15];
cx qubits[17], qubits[67];
ccx qubits[10], qubits[62], qubits[15];
h qubits[50];
cx qubits[68], qubits[35];
t qubits[13];
s qubits[32];
cx qubits[32], qubits[35];
s qubits[60];
z qubits[65];
s qubits[44];
rx(pi/2) qubits[14];
y qubits[15];
t qubits[67];
ry(pi/2) qubits[60];
rx(pi/2) qubits[26];
y qubits[30];
ry(pi/2) qubits[59];
h qubits[58];
y qubits[17];
cx qubits[14], qubits[31];
z qubits[58];
ry(pi/2) qubits[48];
z qubits[26];
x qubits[40];
y qubits[57];
z qubits[40];
y qubits[45];
cx qubits[8], qubits[13];
z qubits[45];
ccx qubits[18], qubits[46], qubits[22];
rx(pi/2) qubits[28];
cx qubits[49], qubits[45];
t qubits[63];
ry(pi/2) qubits[63];
z qubits[42];
t qubits[67];
cz qubits[56], qubits[9];
x qubits[15];
y qubits[26];
rx(pi/2) qubits[16];
ccx qubits[5], qubits[0], qubits[58];
s qubits[10];
z qubits[67];
cz qubits[67], qubits[66];
rx(pi/2) qubits[19];
rx(pi/2) qubits[7];
t qubits[21];
rx(pi/2) qubits[37];
z qubits[15];
x qubits[63];
h qubits[55];
cz qubits[28], qubits[65];
x qubits[25];
ccx qubits[30], qubits[1], qubits[12];
cz qubits[22], qubits[62];
cz qubits[60], qubits[65];
t qubits[57];
x qubits[26];
rx(pi/2) qubits[29];
ry(pi/2) qubits[23];
t qubits[18];
ry(pi/2) qubits[28];
x qubits[23];
t qubits[21];
s qubits[1];
ry(pi/2) qubits[33];
ccx qubits[30], qubits[63], qubits[67];
ccx qubits[24], qubits[20], qubits[47];
rx(pi/2) qubits[51];
y qubits[39];
