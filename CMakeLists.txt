#
# top-level CMakeLists.txt
#
cmake_minimum_required(VERSION 3.6)

# project name
project(the-ray-tracer CXX)

# disable colors : they don't help much
set (CMAKE_COLOR_MAKEFILE 0)

# we are going with c++11
set (CMAKE_CXX_STANDARD 17)

add_compile_options(
  -fdiagnostics-color=never
  -g
  -O2
  -march=native
  -std=c++17
  )

#
# all the subdirs making up this project i.e the following:
#    - 3rd-party/doctest   : for tests
#    - common              : commonly used utilities
#    - src                 : actual sources
#    - test                : the tests
#
add_subdirectory(3rd-party/doctest)
add_subdirectory(common)
add_subdirectory(src)
add_subdirectory(test)
