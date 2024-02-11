# Project template

Modified from: [modern cmake](https://gitlab.com/CLIUtils/modern-cmake/-/blob/master/examples/extended-project/CMakeLists.txt)

```bash
# need system to install gtest: sudo apt-get install libgtest-dev
mkdir build && cd build && cmake ..
make -j 10
ctest --verbose
```
