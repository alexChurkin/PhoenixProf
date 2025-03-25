include("../../build_utils/CMakeLists.txt")
SetRequiredCMakeVersion()
cmake_minimum_required(VERSION ${REQUIRED_CMAKE_VERSION})

project(PhoenixProf_Samples_OpenCL_GEMM CXX)
SetCompilerFlags()
SetBuildType()

add_executable(cl_gemm main.cc)
target_include_directories(cl_gemm
  PRIVATE "${PROJECT_SOURCE_DIR}/../k_utils")
if(CMAKE_INCLUDE_PATH)
  target_include_directories(cl_gemm
    PUBLIC "${CMAKE_INCLUDE_PATH}")
endif()

FindOpenCLLibrary(cl_gemm)
FindOpenCLHeaders(cl_gemm)