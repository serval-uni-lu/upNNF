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
