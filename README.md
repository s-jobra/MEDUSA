# MEDUSA: An MTBDD-based quantum circuit simulator
MEDUSA (**M**ulti-Terminal Binary D**E**cision **D**iagram-based Q**U**antum **S**imul**A**tor) is an MTBDD-based quantum circuit simulator supporting `OpenQASM` circuits.
It is written in C and utilizes the [Sylvan](https://trolando.github.io/sylvan/) library for custom MTBDDs and MTBDD operations.

## Build
**Dependencies:**
* `gmp` library
* [`Sylvan`](https://trolando.github.io/sylvan/) and therefore [`Lace`](https://fmt.ewi.utwente.nl/tools/lace/) libraries - can be installed via `make` (needs `git`), see below for more details

Before trying to build this project, make sure to have `gmp` library (`libgmp-dev`) installed on your machine.
Then you can download the remaining dependencies with:
```
make install-deps
```
You can then build the project with:
```
make
```

## Usage
The simulator accepts path to the circuit file with `OpenQASM` format either as a program argument or from the standard input, such as:
```
./sim examples/bell.qasm 
```
```
./sim <examples/bell.qasm 
```
You can also run the simulator with the flag `--time` to print runtime (wall-clock time) to the standard output.

The result of the simulation can be found in file `res.dot`. Beware that as these files can be quite large, it can take a while to convert them into a graph with a tool such as [`Graphviz`](https://graphviz.org/).

You can also download benchmark files with:
```
make get-benchmarks
```