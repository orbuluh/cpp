# Random Cpp related learning notes
- [Boost quick reference](boost/README.md)
- [Concurrency](concurrency/README.md)
- [Design pattern with my practices in cpp](designPattern/README.md)
- [Random cpp language/std groceries](std-Aha/README.md)
- [Template (meta) related](template-meta/README.md)

# Quick reference for cmake
```
# after clone, at folder root
mkdir build
cd build
cmake .. #compile build folder for cmake
# <NOTE: DON'T TRY TO RUN cmake . without cd into build!!!>

#(re)build going forward
cmake --build build

# then run program
./build/PlayGround [-d {boost|std|test}]
# or run
ctest
```