OPENQASM 2.0;
include "qelib1.inc";
qreg qubits[25];
creg c[25];

cx qubits[11], qubits[13];
ccx qubits[11], qubits[12], qubits[13];
cx qubits[12], qubits[13];
cx qubits[10], qubits[14];
ccx qubits[10], qubits[13], qubits[14];
cx qubits[13], qubits[14];
cx qubits[9], qubits[15];
ccx qubits[9], qubits[14], qubits[15];
cx qubits[14], qubits[15];
cx qubits[8], qubits[16];
ccx qubits[8], qubits[15], qubits[16];
cx qubits[15], qubits[16];
cx qubits[7], qubits[17];
ccx qubits[7], qubits[16], qubits[17];
cx qubits[16], qubits[17];
cx qubits[6], qubits[18];
ccx qubits[6], qubits[17], qubits[18];
cx qubits[17], qubits[18];
ccx qubits[5], qubits[18], qubits[19];
ccx qubits[4], qubits[19], qubits[20];
ccx qubits[3], qubits[20], qubits[21];
ccx qubits[2], qubits[21], qubits[22];
ccx qubits[1], qubits[22], qubits[23];
cx qubits[0], qubits[23];
x qubits[23];
cx qubits[1], qubits[22];
x qubits[22];
cx qubits[2], qubits[21];
x qubits[21];
cx qubits[3], qubits[20];
x qubits[20];
cx qubits[4], qubits[19];
x qubits[19];
cx qubits[5], qubits[18];
x qubits[18];
cx qubits[6], qubits[17];
x qubits[17];
cx qubits[7], qubits[16];
x qubits[16];
cx qubits[8], qubits[15];
x qubits[15];
cx qubits[9], qubits[14];
x qubits[14];
cx qubits[10], qubits[13];
x qubits[13];
cx qubits[11], qubits[24];
cx qubits[12], qubits[24];
x qubits[23];
x qubits[22];
x qubits[21];
x qubits[20];
x qubits[19];
x qubits[18];
x qubits[12];
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
