include("../build_utils/CMakeLists.txt")
SetRequiredCMakeVersion()
cmake_minimum_required(VERSION ${REQUIRED_CMAKE_VERSION})

project(PhoenixProf_Advanced_Profiler CXX)
SetCompilerFlags()
SetBuildType()

# -- PhoenixProf Tool Library --
add_library(phoenixprof_tool_lib SHARED
  "${PROJECT_SOURCE_DIR}/loader/init.cc"
  tool.cc)
target_include_directories(phoenixprof_tool_lib
  PRIVATE "${PROJECT_SOURCE_DIR}/utils")
target_include_directories(phoenixprof_tool_lib
  PRIVATE "${PROJECT_SOURCE_DIR}/collector")
if(CMAKE_INCLUDE_PATH)
  target_include_directories(phoenixprof_tool_lib
    PUBLIC "${CMAKE_INCLUDE_PATH}")
endif()

FindOpenCLLibrary(phoenixprof_tool_lib)
FindOpenCLHeaders(phoenixprof_tool_lib)
GetOpenCLTracingHeaders(phoenixprof_tool_lib)

# -- Loader --
# 1. Takes input arguments
# 2. Loads tool library via LD_PRELOAD,
# 3. Launches target application (passed as an argument)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOOL_NAME=phoenixprof_tool_lib")
add_executable(cl_hot_opencl "${PROJECT_SOURCE_DIR}/loader/loader.cc")
target_include_directories(cl_hot_opencl
  PRIVATE "${PROJECT_SOURCE_DIR}/utils")
target_include_directories(cl_hot_opencl
  PRIVATE "${PROJECT_SOURCE_DIR}/loader")
if(UNIX)
  target_link_libraries(cl_hot_opencl
    dl)
endif()