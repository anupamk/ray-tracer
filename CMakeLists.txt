# ------------------------------------------------------------------------------
# minimum cmake version required
cmake_minimum_required(VERSION 3.10)

# ------------------------------------------------------------------------------
# project name
project(the-ray-tracer CXX)

# ------------------------------------------------------------------------------
# enable testing
include(CTest)
enable_testing()

# ------------------------------------------------------------------------------
# disable colors : they don't help much
set(CMAKE_COLOR_MAKEFILE 0)

# ------------------------------------------------------------------------------
# we are going with c++17
set(CMAKE_CXX_STANDARD 17)

# ------------------------------------------------------------------------------
# verbose builds
set(CMAKE_VERBOSE_MAKEFILE OFF)

# ------------------------------------------------------------------------------
# create the compilation-database (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# enable (ON) || disable (OFF) execution profiling
set(ENABLE_EXECUTION_PROFILING OFF)

# ------------------------------------------------------------------------------
# 3rd-party code (as submodules) making up this project. these are:
#   - doctest         : for tests
#   - concurrentqueue : concurrent mpmc queue
add_subdirectory(3rd-party)

# ------------------------------------------------------------------------------
# tracy : realtime high-resolution profiler
if (ENABLE_EXECUTION_PROFILING)
  # ----------------------------------------------------------------------------
  # configure tracy features
  set(TRACY_ENABLE ON CACHE BOOL "enable profiling")
  set(TRACY_CALLSTACK ON CACHE BOOL "collect call stacks")

  add_subdirectory(3rd-party/tracy)
endif()

# ------------------------------------------------------------------------------
# our stuff
add_subdirectory(common)
add_subdirectory(src)

# ------------------------------------------------------------------------------
# tests are all together in this sub-directory
add_subdirectory(tests)
