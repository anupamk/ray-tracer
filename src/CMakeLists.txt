# ------------------------------------------------------------------------------
# find xcb package : required for online display of rendering progress
find_package(X11 REQUIRED)

# ==============================================================================
# setup the compiler flags for the project. these flags
# i.e. 'RT_C_...' are defined in the '${CMAKE_TOOLCHAIN_FILE}'
#
# note
# ====
# these are setup here because we want these options to be applied to
# all the sources that we control. we don't want our options inflicted
# on the unsuspecting code living in the '3rd-party'
#
# this is in line with the fundamental principle:
#    'Render unto Ceaser the things that are Ceaser's ... '
# ==============================================================================
SET (CMAKE_C_FLAGS                     "${RT_C_COMMON_FLAGS} ${CMAKE_C_FLAGS}")
SET (CMAKE_C_FLAGS_DEBUG               "${CMAKE_C_FLAGS_DEBUG}")
SET (CMAKE_C_FLAGS_MINSIZEREL          "${RT_C_COMMON_FLAGS} ${RT_C_MIN_SIZE_REL_FLAGS} ${CMAKE_C_FLAGS_MINSIZEREL}")
SET (CMAKE_C_FLAGS_RELEASE             "${RT_C_COMMON_FLAGS} ${RT_C_REL_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO      "${RT_C_COMMON_FLAGS} ${C_FLAGS_RELWITHDEBINFO}")
SET (CMAKE_CXX_FLAGS                   "${RT_CXX_COMMON_FLAGS}")
SET (CMAKE_CXX_FLAGS_DEBUG             "${RT_CXX_COMMON_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
SET (CMAKE_CXX_FLAGS_MINSIZEREL        "${RT_CXX_MIN_SIZE_REL_FLAGS}")
SET (CMAKE_CXX_FLAGS_RELEASE           "${RT_CXX_REL_FLAGS}")

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
# |-- platform-utils    : platform specific common utilities f.e. mmap etc.
# ------------------------------------------------------------------------------
add_subdirectory(common)
add_subdirectory(utils)
add_subdirectory(platform_utils)
add_subdirectory(primitives)
add_subdirectory(shapes)
add_subdirectory(patterns)
add_subdirectory(io)
add_subdirectory(executables)
