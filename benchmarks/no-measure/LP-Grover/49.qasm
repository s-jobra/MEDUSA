OPENQASM 3.0;
include "stdgates.inc";
qubit[98] q;

x q[49];
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
h q[18];
h q[19];
h q[20];
h q[21];
h q[22];
h q[23];
h q[24];
h q[25];
h q[26];
h q[27];
h q[28];
h q[29];
h q[30];
h q[31];
h q[32];
h q[33];
h q[34];
h q[35];
h q[36];
h q[37];
h q[38];
h q[39];
h q[40];
h q[41];
h q[42];
h q[43];
h q[44];
h q[45];
h q[46];
h q[47];
h q[48];
for int i in [1:18634801] {
x q[0];
x q[2];
x q[4];
x q[6];
x q[8];
x q[10];
x q[12];
x q[14];
x q[16];
x q[18];
x q[20];
x q[22];
x q[24];
x q[26];
x q[28];
x q[30];
x q[32];
x q[34];
x q[36];
x q[38];
x q[40];
x q[42];
x q[44];
x q[46];
x q[48];
ccx q[0], q[1], q[50];
ccx q[2], q[50], q[51];
ccx q[3], q[51], q[52];
ccx q[4], q[52], q[53];
ccx q[5], q[53], q[54];
ccx q[6], q[54], q[55];
ccx q[7], q[55], q[56];
ccx q[8], q[56], q[57];
ccx q[9], q[57], q[58];
ccx q[10], q[58], q[59];
ccx q[11], q[59], q[60];
ccx q[12], q[60], q[61];
ccx q[13], q[61], q[62];
ccx q[14], q[62], q[63];
ccx q[15], q[63], q[64];
ccx q[16], q[64], q[65];
ccx q[17], q[65], q[66];
ccx q[18], q[66], q[67];
ccx q[19], q[67], q[68];
ccx q[20], q[68], q[69];
ccx q[21], q[69], q[70];
ccx q[22], q[70], q[71];
ccx q[23], q[71], q[72];
ccx q[24], q[72], q[73];
ccx q[25], q[73], q[74];
ccx q[26], q[74], q[75];
ccx q[27], q[75], q[76];
ccx q[28], q[76], q[77];
ccx q[29], q[77], q[78];
ccx q[30], q[78], q[79];
ccx q[31], q[79], q[80];
ccx q[32], q[80], q[81];
ccx q[33], q[81], q[82];
ccx q[34], q[82], q[83];
ccx q[35], q[83], q[84];
ccx q[36], q[84], q[85];
ccx q[37], q[85], q[86];
ccx q[38], q[86], q[87];
ccx q[39], q[87], q[88];
ccx q[40], q[88], q[89];
ccx q[41], q[89], q[90];
ccx q[42], q[90], q[91];
ccx q[43], q[91], q[92];
ccx q[44], q[92], q[93];
ccx q[45], q[93], q[94];
ccx q[46], q[94], q[95];
ccx q[47], q[95], q[96];
cz q[96], q[48];
ccx q[47], q[95], q[96];
ccx q[46], q[94], q[95];
ccx q[45], q[93], q[94];
ccx q[44], q[92], q[93];
ccx q[43], q[91], q[92];
ccx q[42], q[90], q[91];
ccx q[41], q[89], q[90];
ccx q[40], q[88], q[89];
ccx q[39], q[87], q[88];
ccx q[38], q[86], q[87];
ccx q[37], q[85], q[86];
ccx q[36], q[84], q[85];
ccx q[35], q[83], q[84];
ccx q[34], q[82], q[83];
ccx q[33], q[81], q[82];
ccx q[32], q[80], q[81];
ccx q[31], q[79], q[80];
ccx q[30], q[78], q[79];
ccx q[29], q[77], q[78];
ccx q[28], q[76], q[77];
ccx q[27], q[75], q[76];
ccx q[26], q[74], q[75];
ccx q[25], q[73], q[74];
ccx q[24], q[72], q[73];
ccx q[23], q[71], q[72];
ccx q[22], q[70], q[71];
ccx q[21], q[69], q[70];
ccx q[20], q[68], q[69];
ccx q[19], q[67], q[68];
ccx q[18], q[66], q[67];
ccx q[17], q[65], q[66];
ccx q[16], q[64], q[65];
ccx q[15], q[63], q[64];
ccx q[14], q[62], q[63];
ccx q[13], q[61], q[62];
ccx q[12], q[60], q[61];
ccx q[11], q[59], q[60];
ccx q[10], q[58], q[59];
ccx q[9], q[57], q[58];
ccx q[8], q[56], q[57];
ccx q[7], q[55], q[56];
ccx q[6], q[54], q[55];
ccx q[5], q[53], q[54];
ccx q[4], q[52], q[53];
ccx q[3], q[51], q[52];
ccx q[2], q[50], q[51];
ccx q[0], q[1], q[50];
x q[0];
x q[2];
x q[4];
x q[6];
x q[8];
x q[10];
x q[12];
x q[14];
x q[16];
x q[18];
x q[20];
x q[22];
x q[24];
x q[26];
x q[28];
x q[30];
x q[32];
x q[34];
x q[36];
x q[38];
x q[40];
x q[42];
x q[44];
x q[46];
x q[48];
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
h q[18];
h q[19];
h q[20];
h q[21];
h q[22];
h q[23];
h q[24];
h q[25];
h q[26];
h q[27];
h q[28];
h q[29];
h q[30];
h q[31];
h q[32];
h q[33];
h q[34];
h q[35];
h q[36];
h q[37];
h q[38];
h q[39];
h q[40];
h q[41];
h q[42];
h q[43];
h q[44];
h q[45];
h q[46];
h q[47];
h q[48];
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
x q[13];
x q[14];
x q[15];
x q[16];
x q[17];
x q[18];
x q[19];
x q[20];
x q[21];
x q[22];
x q[23];
x q[24];
x q[25];
x q[26];
x q[27];
x q[28];
x q[29];
x q[30];
x q[31];
x q[32];
x q[33];
x q[34];
x q[35];
x q[36];
x q[37];
x q[38];
x q[39];
x q[40];
x q[41];
x q[42];
x q[43];
x q[44];
x q[45];
x q[46];
x q[47];
x q[48];
ccx q[0], q[1], q[50];
ccx q[2], q[50], q[51];
ccx q[3], q[51], q[52];
ccx q[4], q[52], q[53];
ccx q[5], q[53], q[54];
ccx q[6], q[54], q[55];
ccx q[7], q[55], q[56];
ccx q[8], q[56], q[57];
ccx q[9], q[57], q[58];
ccx q[10], q[58], q[59];
ccx q[11], q[59], q[60];
ccx q[12], q[60], q[61];
ccx q[13], q[61], q[62];
ccx q[14], q[62], q[63];
ccx q[15], q[63], q[64];
ccx q[16], q[64], q[65];
ccx q[17], q[65], q[66];
ccx q[18], q[66], q[67];
ccx q[19], q[67], q[68];
ccx q[20], q[68], q[69];
ccx q[21], q[69], q[70];
ccx q[22], q[70], q[71];
ccx q[23], q[71], q[72];
ccx q[24], q[72], q[73];
ccx q[25], q[73], q[74];
ccx q[26], q[74], q[75];
ccx q[27], q[75], q[76];
ccx q[28], q[76], q[77];
ccx q[29], q[77], q[78];
ccx q[30], q[78], q[79];
ccx q[31], q[79], q[80];
ccx q[32], q[80], q[81];
ccx q[33], q[81], q[82];
ccx q[34], q[82], q[83];
ccx q[35], q[83], q[84];
ccx q[36], q[84], q[85];
ccx q[37], q[85], q[86];
ccx q[38], q[86], q[87];
ccx q[39], q[87], q[88];
ccx q[40], q[88], q[89];
ccx q[41], q[89], q[90];
ccx q[42], q[90], q[91];
ccx q[43], q[91], q[92];
ccx q[44], q[92], q[93];
ccx q[45], q[93], q[94];
ccx q[46], q[94], q[95];
ccx q[47], q[95], q[96];
cz q[96], q[48];
ccx q[47], q[95], q[96];
ccx q[46], q[94], q[95];
ccx q[45], q[93], q[94];
ccx q[44], q[92], q[93];
ccx q[43], q[91], q[92];
ccx q[42], q[90], q[91];
ccx q[41], q[89], q[90];
ccx q[40], q[88], q[89];
ccx q[39], q[87], q[88];
ccx q[38], q[86], q[87];
ccx q[37], q[85], q[86];
ccx q[36], q[84], q[85];
ccx q[35], q[83], q[84];
ccx q[34], q[82], q[83];
ccx q[33], q[81], q[82];
ccx q[32], q[80], q[81];
ccx q[31], q[79], q[80];
ccx q[30], q[78], q[79];
ccx q[29], q[77], q[78];
ccx q[28], q[76], q[77];
ccx q[27], q[75], q[76];
ccx q[26], q[74], q[75];
ccx q[25], q[73], q[74];
ccx q[24], q[72], q[73];
ccx q[23], q[71], q[72];
ccx q[22], q[70], q[71];
ccx q[21], q[69], q[70];
ccx q[20], q[68], q[69];
ccx q[19], q[67], q[68];
ccx q[18], q[66], q[67];
ccx q[17], q[65], q[66];
ccx q[16], q[64], q[65];
ccx q[15], q[63], q[64];
ccx q[14], q[62], q[63];
ccx q[13], q[61], q[62];
ccx q[12], q[60], q[61];
ccx q[11], q[59], q[60];
ccx q[10], q[58], q[59];
ccx q[9], q[57], q[58];
ccx q[8], q[56], q[57];
ccx q[7], q[55], q[56];
ccx q[6], q[54], q[55];
ccx q[5], q[53], q[54];
ccx q[4], q[52], q[53];
ccx q[3], q[51], q[52];
ccx q[2], q[50], q[51];
ccx q[0], q[1], q[50];
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
x q[13];
x q[14];
x q[15];
x q[16];
x q[17];
x q[18];
x q[19];
x q[20];
x q[21];
x q[22];
x q[23];
x q[24];
x q[25];
x q[26];
x q[27];
x q[28];
x q[29];
x q[30];
x q[31];
x q[32];
x q[33];
x q[34];
x q[35];
x q[36];
x q[37];
x q[38];
x q[39];
x q[40];
x q[41];
x q[42];
x q[43];
x q[44];
x q[45];
x q[46];
x q[47];
x q[48];
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
h q[18];
h q[19];
h q[20];
h q[21];
h q[22];
h q[23];
h q[24];
h q[25];
h q[26];
h q[27];
h q[28];
h q[29];
h q[30];
h q[31];
h q[32];
h q[33];
h q[34];
h q[35];
h q[36];
h q[37];
h q[38];
h q[39];
h q[40];
h q[41];
h q[42];
h q[43];
h q[44];
h q[45];
h q[46];
h q[47];
h q[48];
z q[49];
}
