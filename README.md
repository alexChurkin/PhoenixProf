# PhoenixProf – Advanced Intel Profiler
<img src="assets/logo.jpg" alt="Phoenix logo" width="256"/>

## Project Structure
The project consists of:
- Tool (**phoenixprof/tool**) is a shared library that can be dynamically loaded/unloaded by the loader
- Loader (**phoenixprof/loader**) module manages program startup, dynamic library initialization and target kernel launch.

## Build
``` bash
cd <path>/phoenixprof
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
make
```