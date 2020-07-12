#
# top-level CMakeLists.txt
#
cmake_minimum_required(VERSION 3.6)

# project name
project(the-ray-tracer CXX)

# disable colors : they don't help much
set (CMAKE_COLOR_MAKEFILE 0)

# we are going with c++17
set (CMAKE_CXX_STANDARD 17)

# verbose builds
set(CMAKE_VERBOSE_MAKEFILE OFF)

# create the compilation-database (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -fdiagnostics-color=never
  -g
  -O2
  -pipe
  -std=c++17
  -Wall
  -Werror
  -lm
  -march=native
  )

#
# all the subdirs making up this project i.e the following:
#    - 3rd-party/doctest   : for tests
#    - common              : commonly used utilities
#    - src                 : actual sources
#    - test                : the tests
#
add_subdirectory(3rd-party)
add_subdirectory(common)
add_subdirectory(src)
add_subdirectory(test)
