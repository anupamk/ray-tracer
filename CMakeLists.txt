#
# top-level CMakeLists.txt
cmake_minimum_required(VERSION 3.6)

# project name
project(the-ray-tracer CXX)

# ------------------------------------------------------------------------------
# enable testing
include(CTest)
enable_testing()

# disable colors : they don't help much
set(CMAKE_COLOR_MAKEFILE 0)

# we are going with c++17
set(CMAKE_CXX_STANDARD 17)

# verbose builds
set(CMAKE_VERBOSE_MAKEFILE OFF)

# create the compilation-database (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# all the subdirs making up this project i.e the following:
#    - 3rd-party
#         - doctest         : for tests (header-only library)
#         - concurrentqueue : concurrent mpmc queue (header-only library)
#         - tracy           : realtime high resolution profiler
#    - common               : commonly used utilities
#    - src                  : actual sources
add_subdirectory(3rd-party)
add_subdirectory(3rd-party/tracy)
add_subdirectory(common)
add_subdirectory(src)

# ------------------------------------------------------------------------------
# tests are all together in this sub-directory
add_subdirectory(tests)
