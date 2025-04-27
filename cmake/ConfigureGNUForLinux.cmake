# -*- cmake -*-

# ==============================================================================
# setup the gnu-tool-chain binaries
# ==============================================================================
SET (CMAKE_C_COMPILER               "/usr/bin/gcc-14")
SET (CMAKE_CXX_COMPILER             "/usr/bin/g++-14")
SET (CMAKE_LINKER                   "/usr/bin/ld")
SET (CMAKE_AR                       "/usr/bin/gcc-ar-14")
SET (CMAKE_NM                       "/usr/bin/gcc-nm-14")
SET (CMAKE_OBJDUMP                  "/usr/bin/objdump")
SET (CMAKE_RANLIB                   "/usr/bin/ranlib")

# hopefully not much of a change below this line is needed
# ==============================================================================

# ------------------------------------------------------------------------------
# include-what-you-use: the right amount of includes in sources for
# FEARLESS development !
#
# see: https://github.com/include-what-you-use/include-what-you-use
# for some more information, and the reasons why this becomes useful.
SET (ENABLE_INCLUDE_WHAT_YOU_USE OFF)

if (ENABLE_INCLUDE_WHAT_YOU_USE)
  SET (CMAKE_CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use;-Xiwyu;--comment_style=none;")
endif()

# ==============================================================================
# set gcc options
# ==============================================================================
string (CONCAT RT_C_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c11"                   # _C_FLAGS_
  " -pipe"
  " -fno-diagnostics-color"
  " -Wstrict-prototypes"
  " -Wold-style-definition"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RT_C_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RT_C_REL_FLAGS
  " -O3"
  " -march=native"
  " -ffast-math"

  # add more _CFLAGS_ there ↑
)

# ==============================================================================
# set g++ options
# ==============================================================================
string (CONCAT RT_CXX_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c++17"                 # _CXX_FLAGS_
  " -pipe"
  " -fno-diagnostics-color"

  # add more _CXXFLAGS_ there ↑
)

string (CONCAT RT_CXX_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RT_CXX_REL_FLAGS
  " -O3"
  " -march=native"
  " -ffast-math"

  # add more _CFLAGS_ there ↑
)

# ==============================================================================
# set linker options
# ==============================================================================
SET (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
SET (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
