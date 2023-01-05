# Intel® CPU Frequency Library
A simple library to sample the frequency on single CPU cores.
The frequency is sampled by computing the ratio of actual performed cycles to the cycles that have passed in base frequency according to rdtsc.


# Build
As buildsystem we use cmake: 
```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install_dir ..
cmake --build . --config Release
cmake --install .
```


# Usage

To include the library in CMake:
```cmake
find_package(cpufreqlib   
              HINTS 
              /path/to/install_dir/cmake)
target_link_libraries(your_target
                      PRIVATE cpufreqlib)             
```
In order to sample the current CPU frequency call:
```cpp
#include <cpufreqlib.h>
// ...
float base_freq = get_base_cpu_freq_hz(); // get base frequency
// ...
float freq = get_curr_cpu_freq_hz(); // get current frequency
```
**Note that SMT might lead to false measurements**
