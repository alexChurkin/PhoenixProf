include("../build_utils/CMakeLists.txt")
SetRequiredCMakeVersion()
cmake_minimum_required(VERSION ${REQUIRED_CMAKE_VERSION})

project(PhoenixProf_Advanced_Profiler CXX)
SetCompilerFlags()
SetBuildType()

# -- PhoenixProf Tool Library --
add_library(phoenixprof_tool_lib SHARED
  "${PROJECT_SOURCE_DIR}/tool/init.cc"
  "${PROJECT_SOURCE_DIR}/tool/tool.cc")
target_include_directories(phoenixprof_tool_lib
  PRIVATE "${PROJECT_SOURCE_DIR}/utils")
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
# 3. Launches target application (that was passed as an argument)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOOL_NAME=phoenixprof_tool_lib")
add_executable(phoenixprof "${PROJECT_SOURCE_DIR}/loader/loader.cc")
target_include_directories(phoenixprof
  PRIVATE "${PROJECT_SOURCE_DIR}/utils")
target_include_directories(phoenixprof
  PRIVATE "${PROJECT_SOURCE_DIR}/loader")
if(UNIX)
  target_link_libraries(phoenixprof
    dl)
endif()