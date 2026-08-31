dependencies:

```
apt-get -y install g++ make zlib1g-dev libboost-dev libgmp-dev libgmpxx4ldbl ninja-build libboost-random-dev libboost-program-options-dev cmake python3-dev
```

build:

```
cd dependencies/networkit
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
```
