To utilize the generator, kindly execute the following command:

```
python generator_PF.py <output_file_name> <nQ1> <nQ2> <nGate> <seed>
```

Here's an explanation of the parameters:

- `<output_file_name>` is the desired name for the generated qasm file.
- `<nQ1 (nQ2)>` is the number of qubits in the first (second) register.
- `<nGate>` is the count of Toffoli gates in the random oracle.
- `<seed>` is used as the seed for generating the random oracle.

Some pre-generated files are available with the naming convention ```PF_<nQ1>_<nQ2>_<nGate>_<seed>.qasm```.

Additional Notes:
1. The inverse QFT part is not included.
2. The oracle is randomly generated without specific functions.
3. The initial state of the second register is set to be |1⟩.
4. For each Toffoli gate, each qubit in the second register (except the target qubit) has the following probabilities: a 25% chance to be a positive control qubit, a 25% chance to be a negative control qubit, and a 50% chance not to be a control qubit.
5. Controlled oracles are separated by barriers.
6. The basis gate set is expected to be {x, y, z, h, s, sdg, t, tdg, cx, mcx}.