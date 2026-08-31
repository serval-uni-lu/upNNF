Dependencies:

```
apt-get -y install g++ make zlib1g-dev libgmp-dev libgmpxx4ldbl libboost-random-dev libboost-dev ninja-build libboost-program-options-dev
```


build:

```
cd dependencies/cadical-rel-2.1.3
./configure
make -j
cd ../../

g++ gen.cpp -o gen
./gen
ninja
```
