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

> [!Note]
> Building networkit can use a significant amount of memory. Reducing the number of processes
> by modifying the line `make -j4` and can drastically reduce memroy usage.

In this example, we install the `d4` executable at `$HOME/.local/bin/d4`.
To modify this, the path needs to be given to the `gen` exectuable in the `capkc` folder when generating the `build.ninja` file and the `d4` executable needs to be moved to the appropriate path.

Given that compilation can require large amounts of memory, an apptainer script
is proposed in the `D4` directory. The container will limit `D4` to 64 GB of memory and
five hours of computation by using the program available at `D4/wrapper`.
To build it, please run the following command in the `D4` directory.

```
apptainer build --fakeroot d4.sif d4.def
```

The resulting container (`d4.sif`) can be used just like the native executable.

The `D4` folder also contains an additional modified version of `D4`, namely `d4d` which has an additional parameter to limit the depth of the compilation.
If the option `-max-depth` is set to the value `n`, then `d4` will stop the compilation of the sub-graph if it has encountered `n` disjunction nodes along the current branch and replace the sub-graph by a `TRUE` node. Therefore, the constructed d-DNNF is an upper bound of the input formula but is built very differently than with `capkc`.

### Example Usage

Suppose we would like to compile a formula named `t.cnf`.
We begin by calling the search algorithm:

```
./capkc/build/capkc --cnf t.cnf
d4 -dDNNF t.cnf.up -out=t.cnf.unnf
```

To use the approximate model counting algorithm with 10000 samples, you may then run:
```
./capkc/build/appmc --cnf t.cnf --nb 10000
```

In this case it returned the following:
```
c Initializing
c Using unnf
N,nbs,Y,Yl,Yh
10000, 10000, 26256, 26238.6, 26256
```

This means that the algorithm did `10000` tries (columnd `N`) and had `10000` successes (column `nbs`).
The estimate is `26256` (columnd `Y`) and the lower and upper bounds are `26238.6` (column `Yl`) and 26256 (column `Yh`) respectively.

Similarily, to use the sampler we use:
```
./capkc/build/sampler --cnf t.cnf --n 2 --print-samples
```

To generate the samples and print them to the screen.
Samples are the only lines that don't start with a `c` and are therefore easy to separate from the remaining output.

> [!NOTE]
> The sampler and approximate model counting algorithm get called with the original path to the cnf file (for example `t.cnf`) but expect the files `t.cnf.smp` and `t.cnf.unnf` to exist. The file `t.cnf.smp` is just the same formula as `t.cnf` but with BCP applied which can be done with `capkc/build/smp` but it can also be the same file as the original. `t.cnf.unnf` is expected to be a d-DNNF generated with the version of `D4` shipped in this repository as it has been modified to output the constrained and unconstrained variables on each edge. With these files provided, the sampler or approximate model counters do not care how the files have been generated. Therefore if compiling `t.cnf` directly to d-DNNF works, then the sampler can be used as a uniform random sampler just like `KUS`, provided the file names are correct.


> [!NOTE]
> To use a different knowledge compiler than the one shipped in this repository, please make sure that the file format is the same and that the new compiler also outputs the unconstrained variables on the d-DNNF edges.

# Additional programs

## Up

`capkc` also contains a program called `up` which we used to remove clauses to simple formulae to perform the uniformity tests with `capkc/build/sampler`.

To remove the last `25` clauses according to the community clause ordering, you may run:
```
./capkc/build/up --cnf t.cnf --n 25
d4 -dDNNF t.cnf.up -out=t.cnf.unnf
```

Similarily to `capkc`, there are two other possible clause ordering that can be set with `--ordering`: `ascending` and `random`.
The `ascending` ordering orders the clauses by ascending literal count. Therefore, using this option would remove the `25` clauses with the most literals.
The default is the `community` ordering

Compilation is then done just like with `capkc`.

Approximate counting:
```
./capkc/build/appmc --cnf t.cnf --nb 10000
```

We then obtain:
```
c Initializing
c Using unnf
N,nbs,Y,Yl,Yh
10000, 155, 27776, 22622.5, 34080.8
```

Or uniform random sampling:
```
./capkc/build/sampler --cnf t.cnf --n 2 --print-samples
```

Both work the same between `capkc` and `up`. This shows that `appmc` and `sampler` can be used outside of the `capkc` framework, provided the expected files are present and have the adequate data and format.

# DNF

The `dnf` folder additionally contains a cube generator.
Basically, it generate a partial `DNF` with a heuristic.
The idea is that we can combine the upper bound returned by `up` or `capkc` (which are built from the bottom up) with cubes that target clauses that have been excluded by the compilation process in a top-down approach and build a tighter d-DNNF by using the `divkc` principle.
In this case however, we wish to avoid the issue where the projected formula had too many solutions, therefore, partial DNF compilation allows us to top the process at any time.
The program starts with one empty cubes and iteratively uses shannon decomposition to split the cube that satisfies the fewest clauses of the input formula with a focus on the ones that are not present in `t.cnf.up` or `t.cnf.unnf`.

## Build

```
cd dnf
cd dependencies/cadical-rel-2.1.3
./configure
make -j
cd ../../

g++ gen.cpp -o gen
./gen
ninja
cd ..
```

## Usage

Example usage:
```
./capkc/build/up --cnf t.cnf --n 25
d4 -dDNNF t.cnf.up -out=t.cnf.unnf
```

In our example, this generates a d-DNNF with `1792000` solutions. The idea is now to use cube generation to further reduce the number of solutions in a predicatble way. In this example we will use at most 10 cubes.
```
./dnf/build/dnf --cnf t.cnf --n 10
```
The command generates at most 10 cubes because the program will terminate early (and write the computed cubes as they are) if either the timeout is reached (`--timeout`) or if the clauses in `t.cnf.ign` (i.e., the clause not in `t.cnf.up` and `t.cnf.unnf` or G_U) are all satisfied by each cube, in which case we can use the DivKC theorems to get an exact d-DNNF instead of an upper bound.

This generates a `t.cnf.cubes` file which can then be used with the sampler and approximate model counter as follows:

```
./capkc/build/appmc --cnf t.cnf --nb 10000 --cubes
```

```
c Initializing
c Using cubes
N,nbs,Y,Yl,Yh
10000, 1676, 26672.5, 25176.6, 28238.6
```

For comparison, the same command without cubes:
```
./capkc/build/appmc --cnf t.cnf --nb 10000
```

```
c Initializing
c Using unnf
N,nbs,Y,Yl,Yh
10000, 164, 29388.8, 24074.7, 35852.1
```

We can immediately see that the number of successes jumped from `164` to `1676` when using cubes.

Similarily for sampling:

```
./capkc/build/sampler --cnf t.cnf --n 2 
```

```
c Initializing
c Using unnf
c Sampling
c UMC 1792000
c nb_tries 108
c nb_success 2
c AMC 33185.2
```

Witout cubes, we see that `t.cnf.unnf` has `1792000` models and that the sampler tried `108` different solutions to find `2` solutions to `t.cnf`.

```
./capkc/build/sampler --cnf t.cnf --n 2 --cubes
```

```
c Initializing
c Using cubes
c Sampling
c UMC 159144
c nb_tries 15
c nb_success 2
c AMC 21219.2
```

With cubes, the upper bound (built with the `divkc` theorems) has `159144` (about `11` times less for only `10` cubes) and the number of tries by the sampler plummeted from `108` to `15` with the usage of cubes.

Also, notice the lines `c Using unnf` which indicates the use of the upper bound and `c Using cubes` indicating the use of the cubes generated by `dnf`.

# Docker

A Dockerfile is also provided.
The image can be build with `docker build -t capkc .`.
The executable files are then available in `/capkc` on the container.
To compile a formula `t.cnf` by using the docker container you can either use the provided
`docker_compile_formula.sh` bash script or you can run the individual commands
manually to retain more control over the program options:

```
docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/capkc --cnf "t.cnf"

docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/wrap 16000 3600 \
    /capkc/d4 -dDNNF "t.cnf.up" -out="t.cnf.unnf"
```

Notice that we used the wrapper in the docker image to compile the formula to d-DNNF
with `D4`.
The wrapper is used as follows:
```
wrap <max memory in MB> <max time in seconds> path-to-executable executable-options
```
Therefore, we in this example, we limit `D4` to 16000 MB of memory and 3600 seconds of
computation. The advantage of using the wrapper is that it outputs a line
such as
```
/capkc/d4 -dDNNF t.cnf.up -out=t.cnf.unnf, done, 575762, 0.0247213
```
which are directly used in the raw `csv` files of our result folder.
The output is as follows: `command, status, memory in KB, time in seconds`.
The status can be `done` in case of success or `err` for errors and
`timeout` or `mem` for timeouts or out of memory early terminations.

Once the compilation has been done, we can perform approximate model counting as follows:
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/appmc --cnf t.cnf
```

Similarily, we can perform random sampling with `/capkc/sampler`:
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/sampler --cnf t.cnf --n 2 --print-samples
```

Cube generation is also available in the docker image with `/capkc/dnf`:
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/dnf --cnf t.cnf --n 10
```

As well as `up`:
```
docker run --rm -v "$(pwd):/work:Z" -w "/work" capkc \
    /capkc/up --cnf t.cnf --n 25
```

# Apptainer

More information on apptainer can be obtained on the [apptainer website](https://apptainer.org/).

Apptainer container scripts are also provided, to build them, please use:
```
cd capkc
apptainer build --fakeroot appmc.sif appmc.def
apptainer build --fakeroot capkc.sif capkc.def
apptainer build --fakeroot sampler.sif sampler.def
apptainer build --fakeroot smp.sif smp.def
apptainer build --fakeroot up.sif up.def
cd ../

cd dnf
apptainer build --fakeroot dnf.sif dnf.def
cd ../

cd D4
apptainer build --fakeroot d4.sif d4.def
cd ../
```

The `*.sif` files can then be used like normal executables:
```
./capkc/capkc.sif --cnf t.cnf
./D4/d4.sif -dDNNF t.cnf.up -out=t.cnf.unnf
./capkc/sampler.sif --cnf t.cnf --n 2 
./capkc/appmc.sif --cnf t.cnf --nb 10000

./capkc/up.sif --cnf t.cnf --n 25
./D4/d4.sif -dDNNF t.cnf.up -out=t.cnf.unnf
./dnf/dnf.sif --cnf t.cnf --n 10
./capkc/sampler.sif --cnf t.cnf --n 2 --cubes
./capkc/appmc.sif --cnf t.cnf --nb 10000 --cubes
```

> [!Note]
> It is possible that the `capkc` compilations are either too slow or run out of memory.
> Adjusting the number of processes used to compile networkit (line 25 in `capkc.def`: `make -j 4`, similar line numbers for the other files in the `capkc` folder)
> by reducing the number of processes can help with memory isses (and will make compilation slower).
