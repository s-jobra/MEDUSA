OPENQASM 2.0;
include "qelib1.inc";
qreg q[26];
x q[13];
h q[0];
h q[1];
h q[2];
h q[3];
h q[4];
h q[5];
h q[6];
h q[7];
h q[8];
h q[9];
h q[10];
h q[11];
h q[12];
for int i in [1:71:1] {
x q[0];
x q[2];
x q[4];
x q[6];
x q[8];
x q[10];
x q[12];
ccx q[0], q[1], q[14];
ccx q[2], q[14], q[15];
ccx q[3], q[15], q[16];
ccx q[4], q[16], q[17];
ccx q[5], q[17], q[18];
ccx q[6], q[18], q[19];
ccx q[7], q[19], q[20];
ccx q[8], q[20], q[21];
ccx q[9], q[21], q[22];
ccx q[10], q[22], q[23];
ccx q[11], q[23], q[24];
cz q[24], q[12];
ccx q[11], q[23], q[24];
ccx q[10], q[22], q[23];
ccx q[9], q[21], q[22];
ccx q[8], q[20], q[21];
ccx q[7], q[19], q[20];
ccx q[6], q[18], q[19];
ccx q[5], q[17], q[18];
ccx q[4], q[16], q[17];
ccx q[3], q[15], q[16];
ccx q[2], q[14], q[15];
ccx q[0], q[1], q[14];
x q[0];
x q[2];
x q[4];
x q[6];
x q[8];
x q[10];
x q[12];
h q[0];
h q[1];
h q[2];
h q[3];
h q[4];
h q[5];
h q[6];
h q[7];
h q[8];
h q[9];
h q[10];
h q[11];
h q[12];
x q[0];
x q[1];
x q[2];
x q[3];
x q[4];
x q[5];
x q[6];
x q[7];
x q[8];
x q[9];
x q[10];
x q[11];
x q[12];
ccx q[0], q[1], q[14];
ccx q[2], q[14], q[15];
ccx q[3], q[15], q[16];
ccx q[4], q[16], q[17];
ccx q[5], q[17], q[18];
ccx q[6], q[18], q[19];
ccx q[7], q[19], q[20];
ccx q[8], q[20], q[21];
ccx q[9], q[21], q[22];
ccx q[10], q[22], q[23];
ccx q[11], q[23], q[24];
cz q[24], q[12];
ccx q[11], q[23], q[24];
ccx q[10], q[22], q[23];
ccx q[9], q[21], q[22];
ccx q[8], q[20], q[21];
ccx q[7], q[19], q[20];
ccx q[6], q[18], q[19];
ccx q[5], q[17], q[18];
ccx q[4], q[16], q[17];
ccx q[3], q[15], q[16];
ccx q[2], q[14], q[15];
ccx q[0], q[1], q[14];
x q[0];
x q[1];
x q[2];
x q[3];
x q[4];
x q[5];
x q[6];
x q[7];
x q[8];
x q[9];
x q[10];
x q[11];
x q[12];
h q[0];
h q[1];
h q[2];
h q[3];
h q[4];
h q[5];
h q[6];
h q[7];
h q[8];
h q[9];
h q[10];
h q[11];
h q[12];
z q[13];
}