OPENQASM 2.0;
include "qelib1.inc";
qreg q[19];
creg c[19];

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
h q[13];
h q[14];
h q[15];
h q[16];
h q[17];
x q[18];
h q[18];
for j in [0:0:1] {
  mcx q[0], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[0], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[0], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[0], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[0], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[0], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[0], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[0], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[0], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[0], q[16], q[18];
  x q[12];
  mcx q[0], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[0], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[0], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[0], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[0], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[0], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:1:1] {
  mcx q[1], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[1], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[1], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[1], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[1], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[1], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[1], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[1], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[1], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[1], q[16], q[18];
  x q[12];
  mcx q[1], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[1], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[1], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[1], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[1], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[1], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:3:1] {
  mcx q[2], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[2], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[2], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[2], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[2], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[2], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[2], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[2], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[2], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[2], q[16], q[18];
  x q[12];
  mcx q[2], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[2], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[2], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[2], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[2], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[2], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:7:1] {
  mcx q[3], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[3], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[3], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[3], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[3], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[3], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[3], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[3], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[3], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[3], q[16], q[18];
  x q[12];
  mcx q[3], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[3], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[3], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[3], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[3], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[3], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:15:1] {
  mcx q[4], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[4], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[4], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[4], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[4], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[4], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[4], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[4], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[4], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[4], q[16], q[18];
  x q[12];
  mcx q[4], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[4], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[4], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[4], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[4], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[4], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:31:1] {
  mcx q[5], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[5], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[5], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[5], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[5], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[5], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[5], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[5], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[5], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[5], q[16], q[18];
  x q[12];
  mcx q[5], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[5], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[5], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[5], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[5], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[5], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:63:1] {
  mcx q[6], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[6], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[6], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[6], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[6], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[6], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[6], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[6], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[6], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[6], q[16], q[18];
  x q[12];
  mcx q[6], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[6], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[6], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[6], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[6], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[6], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:127:1] {
  mcx q[7], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[7], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[7], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[7], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[7], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[7], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[7], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[7], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[7], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[7], q[16], q[18];
  x q[12];
  mcx q[7], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[7], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[7], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[7], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[7], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[7], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:255:1] {
  mcx q[8], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[8], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[8], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[8], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[8], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[8], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[8], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[8], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[8], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[8], q[16], q[18];
  x q[12];
  mcx q[8], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[8], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[8], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[8], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[8], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[8], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:511:1] {
  mcx q[9], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[9], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[9], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[9], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[9], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[9], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[9], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[9], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[9], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[9], q[16], q[18];
  x q[12];
  mcx q[9], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[9], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[9], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[9], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[9], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[9], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:1023:1] {
  mcx q[10], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[10], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[10], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[10], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[10], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[10], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[10], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[10], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[10], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[10], q[16], q[18];
  x q[12];
  mcx q[10], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[10], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[10], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[10], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[10], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[10], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
for j in [0:2047:1] {
  mcx q[11], q[14], q[16], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  mcx q[11], q[12], q[14], q[16], q[17], q[18];
  x q[12];
  x q[14];
  x q[16];
  x q[17];
  x q[14];
  mcx q[11], q[15], q[14], q[18];
  x q[14];
  x q[14];
  mcx q[11], q[13], q[15], q[14], q[18];
  x q[14];
  x q[17];
  mcx q[11], q[15], q[17], q[18];
  x q[17];
  x q[12];
  x q[14];
  mcx q[11], q[12], q[14], q[18];
  x q[12];
  x q[14];
  mcx q[11], q[12], q[18];
  x q[16];
  x q[17];
  mcx q[11], q[15], q[16], q[17], q[18];
  x q[16];
  x q[17];
  x q[14];
  x q[16];
  mcx q[11], q[13], q[14], q[16], q[18];
  x q[14];
  x q[16];
  mcx q[11], q[16], q[18];
  x q[12];
  mcx q[11], q[17], q[12], q[18];
  x q[12];
  x q[12];
  x q[16];
  x q[17];
  mcx q[11], q[13], q[15], q[12], q[16], q[17], q[18];
  x q[12];
  x q[16];
  x q[17];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  mcx q[11], q[12], q[13], q[15], q[16], q[18];
  x q[12];
  x q[13];
  x q[15];
  x q[16];
  x q[15];
  x q[16];
  mcx q[11], q[13], q[15], q[16], q[18];
  x q[15];
  x q[16];
  x q[12];
  x q[17];
  mcx q[11], q[14], q[12], q[17], q[18];
  x q[12];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[17];
  mcx q[11], q[12], q[13], q[14], q[15], q[16], q[17];
  h q[17];
  x q[12];
  x q[13];
  x q[14];
  x q[15];
  x q[16];
  x q[17];
  h q[12];
  h q[13];
  h q[14];
  h q[15];
  h q[16];
  h q[17];
}
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];
measure q[4] -> c[4];
measure q[5] -> c[5];
measure q[6] -> c[6];
measure q[7] -> c[7];
measure q[8] -> c[8];
measure q[9] -> c[9];
measure q[10] -> c[10];
measure q[11] -> c[11];
measure q[12] -> c[12];
measure q[13] -> c[13];
measure q[14] -> c[14];
measure q[15] -> c[15];
measure q[16] -> c[16];
measure q[17] -> c[17];
measure q[18] -> c[18];