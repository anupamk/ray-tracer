# ------------------------------------------------------------------------------
# find sdl2 package : required for sdl2_canvas implementation
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

# ------------------------------------------------------------------------------
# we have the following directory organization
# ./src
# |-- CMakeLists.txt
# |-- common            : commonly utilities f.e. logging, asserts etc.
# |-- executables       : all executables
# |-- io                : scene io routines f.e. canvas etc.
# |-- patterns          : various patterns f.e. ring, blended etc.
# |-- primitives        : ray, color, matrix etc. etc.
# |-- shapes            : supported shapes f.e. plane, sphere, cube etc.
# |-- utils             : various utilities f.e. benchmark, constants etc.
# |-- file-utils        : file specific common utilities f.e. mmap etc.
# ------------------------------------------------------------------------------
add_subdirectory(common)
add_subdirectory(utils)
add_subdirectory(file_utils)
add_subdirectory(primitives)
add_subdirectory(shapes)
add_subdirectory(patterns)
add_subdirectory(io)
add_subdirectory(executables)
