# OpenCL(TM) GEMM
## Overview
This sample application performs general matrix multiplication using OpenCL(TM) CPU or GPU device, so it can be used as a target for OpenCL(TM) profiling and tracing tools.
```
OpenCL Matrix Multiplication (matrix size: 512 x 512, repeats 4 times)
Target device: Intel(R) Arc(TM) Graphics
Matrix multiplication time: 0.0030474 sec
Results are CORRECT with accuracy: 2.26974e-06
Matrix multiplication time: 0.00130562 sec
Results are CORRECT with accuracy: 2.26974e-06
Matrix multiplication time: 0.0012725 sec
Results are CORRECT with accuracy: 2.26974e-06
Matrix multiplication time: 0.00184469 sec
Results are CORRECT with accuracy: 2.26974e-06
Total execution time: 0.0264119 sec
```
## Supported OS
- Linux
- Windows

## Prerequisites
- [CMake](https://cmake.org/) (version 3.12 and above)
- [Git](https://git-scm.com/) (version 1.8 and above)
- [Python](https://www.python.org/) (version 2.7 and above)
- [OpenCL(TM) ICD Loader](https://github.com/KhronosGroup/OpenCL-ICD-Loader)
- [Intel(R) Graphics Compute Runtime for oneAPI Level Zero and OpenCL(TM) Driver](https://github.com/intel/compute-runtime)

## Build and Run
### Linux
Run the following commands to build the sample:
```sh
cd <phoenixprof>/samples/cl_gemm
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
Use this command line to run the application:
```sh
./cl_gemm [cpu|gpu|cpugpu] [matrix_size] [repeat_count]
```
### Windows
Use Microsoft* Visual Studio x64 command prompt to run the following commands and build the sample:
```sh
cd <phoenixprof>\samples\cl_gemm
mkdir build
cd build
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_LIBRARY_PATH=<opencl_icd_lib_path> ..
nmake
```
Use this command line to run the application:
```sh
cl_gemm.exe [cpu|gpu|cpugpu] [matrix_size] [repeat_count]
```

#### cpugpu mode runs the app on cpu, and then on gpu.