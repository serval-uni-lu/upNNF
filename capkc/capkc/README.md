build networkit

```
cd dependencies/networkit
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
make -j8
cd ../../../

g++ gen.cpp -o gen
./gen
ninja
```
