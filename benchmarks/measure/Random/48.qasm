OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[48];
creg c[48];

cx qubits[7], qubits[9];
h qubits[17];
s qubits[23];
ccx qubits[11], qubits[28], qubits[30];
cx qubits[3], qubits[1];
y qubits[21];
z qubits[3];
z qubits[39];
x qubits[17];
z qubits[15];
ccx qubits[23], qubits[31], qubits[2];
y qubits[23];
rx(pi/2) qubits[20];
x qubits[41];
x qubits[16];
cz qubits[10], qubits[15];
cz qubits[38], qubits[45];
s qubits[42];
t qubits[44];
ry(pi/2) qubits[14];
x qubits[2];
z qubits[5];
cx qubits[36], qubits[41];
rx(pi/2) qubits[20];
h qubits[35];
t qubits[44];
y qubits[41];
s qubits[42];
y qubits[42];
s qubits[40];
cx qubits[43], qubits[37];
x qubits[5];
z qubits[46];
z qubits[17];
s qubits[22];
ry(pi/2) qubits[19];
ry(pi/2) qubits[11];
rx(pi/2) qubits[19];
h qubits[43];
h qubits[26];
z qubits[26];
cz qubits[16], qubits[32];
cz qubits[41], qubits[42];
x qubits[2];
y qubits[43];
cx qubits[5], qubits[16];
x qubits[20];
cz qubits[28], qubits[18];
cx qubits[3], qubits[5];
h qubits[30];
y qubits[40];
x qubits[24];
s qubits[47];
y qubits[33];
s qubits[14];
cx qubits[44], qubits[24];
y qubits[45];
x qubits[46];
ccx qubits[33], qubits[20], qubits[25];
rx(pi/2) qubits[40];
z qubits[25];
cx qubits[25], qubits[44];
t qubits[46];
t qubits[27];
y qubits[15];
t qubits[45];
x qubits[18];
cz qubits[30], qubits[32];
rx(pi/2) qubits[27];
cx qubits[10], qubits[40];
rx(pi/2) qubits[24];
ccx qubits[36], qubits[44], qubits[41];
cx qubits[4], qubits[23];
ry(pi/2) qubits[30];
h qubits[21];
rx(pi/2) qubits[15];
ry(pi/2) qubits[9];
h qubits[30];
h qubits[35];
cx qubits[46], qubits[33];
x qubits[31];
cz qubits[38], qubits[7];
t qubits[46];
ccx qubits[34], qubits[14], qubits[31];
s qubits[14];
cz qubits[47], qubits[8];
z qubits[16];
rx(pi/2) qubits[15];
y qubits[13];
rx(pi/2) qubits[22];
x qubits[5];
cz qubits[24], qubits[19];
y qubits[35];
x qubits[42];
rx(pi/2) qubits[26];
z qubits[39];
y qubits[38];
ry(pi/2) qubits[18];
x qubits[45];
cz qubits[25], qubits[13];
z qubits[11];
t qubits[6];
h qubits[29];
ccx qubits[19], qubits[3], qubits[46];
ry(pi/2) qubits[28];
cx qubits[11], qubits[6];
z qubits[30];
cz qubits[18], qubits[24];
cz qubits[9], qubits[28];
cz qubits[31], qubits[14];
ry(pi/2) qubits[44];
h qubits[9];
y qubits[23];
h qubits[7];
z qubits[31];
cz qubits[32], qubits[18];
h qubits[30];
cz qubits[4], qubits[21];
ccx qubits[27], qubits[14], qubits[35];
y qubits[33];
y qubits[32];
ry(pi/2) qubits[7];
ry(pi/2) qubits[9];
cz qubits[11], qubits[6];
z qubits[37];
y qubits[43];
cz qubits[38], qubits[40];
h qubits[43];
cz qubits[26], qubits[3];
ry(pi/2) qubits[4];
t qubits[11];
y qubits[2];
cz qubits[45], qubits[14];
h qubits[25];
rx(pi/2) qubits[37];
rx(pi/2) qubits[41];
z qubits[38];
h qubits[5];
ry(pi/2) qubits[9];
y qubits[5];
cx qubits[27], qubits[45];
x qubits[38];
s qubits[38];
cz qubits[27], qubits[42];
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
