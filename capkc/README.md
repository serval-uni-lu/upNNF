# CapKC: Leveragin upper bounds and partial compilation for uniform random sampling

## D4

The folder `D4` contains a slightly modified version of the D4 compiler.
The modification is only present to also include the unconstrained
varables in the generated .nnf file therefore simplifying further
usage of the d-DNNF.

The current implementation in `capkc` requires this modification to function.
Therefore, to replace `D4` by a different compiler, either the compiler
or the implementation in `capkc` has to be modified.

It also contains `d4d`, which has an additional parameter which enables
one to limit the depth of the d-DNNF by counting the disjunction nodes
(conjunction nodes are not counted as depth).

Finally, the folder contains `wrapper`, which is used to limit as well as measure
the time and memory usage of a given program.

## Usage

To use our approach, the `capkc`, and `D4` projects will need to be compiled
by following the instructions in the respective folders.

### Dependencies

The dependencies on Debian-based systems can be installed as follows:
```
apt install g++ make zlib1g-dev libboost-dev libgmp-dev libgmpxx4ldbl ninja-build libboost-program-options-dev libboost-random-dev cmake python3-dev
```

### Building

These commands should build the individual projects. Detailed instructions are
given in each subdirectory.

```
cd capkc

cd ./dependencies/networkit
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
make -j4
cd ../../../

g++ gen.cpp -o gen
./gen -DD4_PATH=\\\""$HOME/.local/bin/d4"\\\"
ninja clean
ninja
cd ..

cd D4/d4
make clean
make -j
cp d4 "$HOME/.local/bin/d4"
cd ../..

cd D4/wrapper
make clean
make -j
cd ../..
```

Given that compilation can require large amounts of memory, an apptainer script
is proposed in the `D4` directory. The container will limit `D4` to 64 GB of memory and
five hours of computation.

```
apptainer build --fakeroot d4.sif d4.def
```

The resulting container (`d4.sif`) can be used just like the native executable.

### Example Usage

Suppose we would like to compile a formula named `t.cnf`.
We begin by calling the search algorithm:

```
./capkc/build/capkc --cnf t.cnf
```
