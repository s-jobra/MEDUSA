OPENQASM 2.0;
include "qelib1.inc";
qreg q[0];
h q[0];
for int i in [1:2:1] {
x q[0];
}
