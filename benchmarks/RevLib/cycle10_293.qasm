OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[39];
creg c[39];

cx qubits[0], qubits[12];
cx qubits[10], qubits[12];
cx qubits[1], qubits[13];
cx qubits[11], qubits[13];
cx qubits[10], qubits[14];
cx qubits[13], qubits[14];
x qubits[13];
ccx qubits[0], qubits[13], qubits[14];
ccx qubits[0], qubits[14], qubits[13];
cx qubits[11], qubits[15];
cx qubits[2], qubits[15];
cx qubits[15], qubits[16];
ccx qubits[1], qubits[2], qubits[16];
ccx qubits[1], qubits[15], qubits[16];
cx qubits[1], qubits[17];
cx qubits[2], qubits[17];
ccx qubits[1], qubits[15], qubits[17];
ccx qubits[1], qubits[2], qubits[17];
cx qubits[17], qubits[18];
ccx qubits[10], qubits[16], qubits[18];
ccx qubits[10], qubits[17], qubits[18];
cx qubits[17], qubits[18];
ccx qubits[0], qubits[18], qubits[17];
cx qubits[2], qubits[19];
cx qubits[3], qubits[19];
cx qubits[11], qubits[20];
cx qubits[3], qubits[20];
ccx qubits[11], qubits[19], qubits[20];
ccx qubits[3], qubits[11], qubits[20];
cx qubits[20], qubits[21];
ccx qubits[1], qubits[19], qubits[21];
ccx qubits[1], qubits[20], qubits[21];
cx qubits[1], qubits[22];
cx qubits[3], qubits[22];
ccx qubits[1], qubits[20], qubits[22];
ccx qubits[1], qubits[3], qubits[22];
cx qubits[22], qubits[23];
ccx qubits[10], qubits[21], qubits[23];
ccx qubits[10], qubits[22], qubits[23];
cx qubits[22], qubits[23];
ccx qubits[0], qubits[23], qubits[22];
ccx qubits[2], qubits[3], qubits[24];
ccx qubits[11], qubits[24], qubits[25];
cx qubits[25], qubits[26];
ccx qubits[1], qubits[24], qubits[26];
ccx qubits[1], qubits[25], qubits[26];
ccx qubits[1], qubits[25], qubits[27];
cx qubits[27], qubits[28];
ccx qubits[10], qubits[26], qubits[28];
ccx qubits[10], qubits[27], qubits[28];
cx qubits[27], qubits[28];
ccx qubits[0], qubits[28], qubits[27];
cx qubits[4], qubits[29];
cx qubits[27], qubits[29];
ccx qubits[4], qubits[27], qubits[30];
cx qubits[5], qubits[31];
cx qubits[30], qubits[31];
ccx qubits[5], qubits[30], qubits[32];
cx qubits[6], qubits[33];
cx qubits[32], qubits[33];
ccx qubits[6], qubits[32], qubits[34];
cx qubits[7], qubits[35];
cx qubits[34], qubits[35];
ccx qubits[7], qubits[34], qubits[36];
cx qubits[8], qubits[37];
cx qubits[36], qubits[37];
ccx qubits[8], qubits[36], qubits[38];
cx qubits[9], qubits[38];
x qubits[38];
x qubits[12];
x qubits[17];
x qubits[22];
x qubits[29];
x qubits[31];
x qubits[33];
x qubits[35];
x qubits[37];
x qubits[38];
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