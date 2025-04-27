# -*- cmake -*-

# ==============================================================================
# setup the llvm-tool-chain binaries
# ==============================================================================
SET (CMAKE_C_COMPILER      "/usr/bin/clang-19")
SET (CMAKE_CXX_COMPILER    "/usr/bin/clang++-19")
SET (CMAKE_AR              "/usr/bin/llvm-ar-19")
SET (CMAKE_NM              "/usr/bin/llvm-nm-19")
SET (CMAKE_OBJDUMP         "/usr/bin/llvm-objdump-19")
SET (CMAKE_RANLIB          "/usr/bin/llvm-ranlib-19")

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
# set clang options
# ==============================================================================
string (CONCAT RT_C_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c11"                   # _C_FLAGS_
  " -pipe"
  " -fno-color-diagnostics"
  " -Wstrict-prototypes"
  " -Wold-style-definition"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RT_C_REL_FLAGS
  " -O3"
  " -march=native"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RT_C_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

# ==============================================================================
# set clang++ options
# ==============================================================================
string (CONCAT RT_CXX_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c++17"
  " -pipe"
  " -fno-color-diagnostics"

  # add more _CXXFLAGS_ there ↑
)

string (CONCAT RT_CXX_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RT_CXX_REL_FLAGS
  " -O3"
  " -march=native"

  # add more _CFLAGS_ there ↑
)
