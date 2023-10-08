#!/usr/bin/python3
import math, sys

n = int(sys.argv[1])
print('OPENQASM 2.0;')
print('include "qelib1.inc";')
print(f'qreg q[{2 * n}];')

print(f'x q[{n}];')
for i in range(n):
    print(f'h q[{i}];')

print(f'for int i in [1:{math.floor(math.pi / (4 * math.asin(1 / pow(2, n/2.0))))}:1] ' + '{')

for i in range(n):
    if (i % 2 == 0):
        print(f'x q[{i}];')
assert(n >= 3)
print(f'ccx q[{0}], q[{1}], q[{n+1}];')
for i in range(3, n): # Note that < does not include n!
    print(f'ccx q[{i-1}], q[{n+i-2}], q[{n+i-1}];')
print(f'cz q[{2*n-2}], q[{n-1}];')
for i in range(n-1, 2, -1):
    print(f'ccx q[{i-1}], q[{n+i-2}], q[{n+i-1}];')
print(f'ccx q[{0}], q[{1}], q[{n+1}];')

for i in range(n):
    if (i % 2 == 0):
        print(f'x q[{i}];')
for i in range(n):
    print(f'h q[{i}];')
for i in range(n):
    print(f'x q[{i}];')
assert(n >= 3)
print(f'ccx q[{0}], q[{1}], q[{n+1}];')
for i in range(3, n): # Note that < does not include n!
    print(f'ccx q[{i-1}], q[{n+i-2}], q[{n+i-1}];')
print(f'cz q[{2*n-2}], q[{n-1}];')
for i in range(n-1, 2, -1):
    print(f'ccx q[{i-1}], q[{n+i-2}], q[{n+i-1}];')
print(f'ccx q[{0}], q[{1}], q[{n+1}];')
for i in range(n):
    print(f'x q[{i}];')
for i in range(n):
    print(f'h q[{i}];')
print(f'z q[{n}];')

print('}')