# Random Cpp learning notes

```
# after clone, at root
mkdir build
cd build
cmake .. #compile build folder for cmake
# <NOTE: DON'T TRY TO RUN cmake . without cd into build!!!>

# going forward
cmake --build build #(re)build

# then run program
./build/PlayGround [-d {dp|boost|test}]
# or run ...
ctest
```