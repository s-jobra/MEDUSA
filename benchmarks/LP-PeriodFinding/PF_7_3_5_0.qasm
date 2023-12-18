OPENQASM 2.0;
include "qelib1.inc";
qreg q[10];
h q[0];
h q[1];
h q[2];
h q[3];
h q[4];
h q[5];
h q[6];
x q[9];
for j in [0:0:1] {
  mcx q[0], q[9], q[8];
  mcx q[0], q[7], q[9];
  mcx q[0], q[8];
  x q[7];
  mcx q[0], q[7], q[9];
  x q[7];
  mcx q[0], q[9];
}
for j in [0:1:1] {
  mcx q[1], q[9], q[8];
  mcx q[1], q[7], q[9];
  mcx q[1], q[8];
  x q[7];
  mcx q[1], q[7], q[9];
  x q[7];
  mcx q[1], q[9];
}
for j in [0:3:1] {
  mcx q[2], q[9], q[8];
  mcx q[2], q[7], q[9];
  mcx q[2], q[8];
  x q[7];
  mcx q[2], q[7], q[9];
  x q[7];
  mcx q[2], q[9];
}
for j in [0:7:1] {
  mcx q[3], q[9], q[8];
  mcx q[3], q[7], q[9];
  mcx q[3], q[8];
  x q[7];
  mcx q[3], q[7], q[9];
  x q[7];
  mcx q[3], q[9];
}
for j in [0:15:1] {
  mcx q[4], q[9], q[8];
  mcx q[4], q[7], q[9];
  mcx q[4], q[8];
  x q[7];
  mcx q[4], q[7], q[9];
  x q[7];
  mcx q[4], q[9];
}
for j in [0:31:1] {
  mcx q[5], q[9], q[8];
  mcx q[5], q[7], q[9];
  mcx q[5], q[8];
  x q[7];
  mcx q[5], q[7], q[9];
  x q[7];
  mcx q[5], q[9];
}
for j in [0:63:1] {
  mcx q[6], q[9], q[8];
  mcx q[6], q[7], q[9];
  mcx q[6], q[8];
  x q[7];
  mcx q[6], q[7], q[9];
  x q[7];
  mcx q[6], q[9];
}
