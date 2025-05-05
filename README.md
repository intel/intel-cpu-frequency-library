# PROJECT NOT UNDER ACTIVE MANAGEMENT #  
This project will no longer be maintained by Intel.  
Intel has ceased development and contributions including, but not limited to, maintenance, bug fixes, new releases, or updates, to this project.  
Intel no longer accepts patches to this project.  
 If you have an ongoing need to use this project, are interested in independently developing it, or would like to maintain patches for the open source software community, please create your own fork of this project.  
  
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

To sample the current frequency of the current core call:
```cpp
#include <cpufreqlib.h>
// ...
float base_freq = get_base_cpu_freq_hz(); // get base frequency
// ...
float freq = get_curr_cpu_freq_hz(); // get current frequency
```
**Note that sampling on two SMT threads of the same physical CPU might lead to false measurements**
