mkdir -p Debug
cd Debug
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
