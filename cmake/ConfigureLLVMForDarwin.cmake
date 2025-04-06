# -*- cmake -*-

# ==============================================================================
# setup the llvm-tool-chain binaries
# ==============================================================================
SET (CMAKE_PREFIX_PATH     "/opt/homebrew")
SET (LLVM_BIN_DIR          "${CMAKE_PREFIX_PATH}/Cellar/llvm/20.1.3/bin/")
SET (CMAKE_C_COMPILER      "${LLVM_BIN_DIR}/clang")
SET (CMAKE_CXX_COMPILER    "${LLVM_BIN_DIR}/clang++")
SET (CMAKE_AR              "${LLVM_BIN_DIR}/llvm-ar")
SET (CMAKE_NM              "${LLVM_BIN_DIR}/llvm-nm")
SET (CMAKE_OBJDUMP         "${LLVM_BIN_DIR}/llvm-objdump")
SET (CMAKE_RANLIB          "${LLVM_BIN_DIR}/llvm-ranlib")

# ------------------------------------------------------------------------------
# this affects how the 'find_*(...)' commands choose between MacOS
# frameworks, and unix-style package components and apple bundles.
SET (CMAKE_FIND_FRAMEWORK NEVER)
SET (CMAKE_FIND_APPBUNDLE NEVER)

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
  " -mcpu=native"

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
  " -mcpu=apple-m4"

  # add more _CFLAGS_ there ↑
)

# ==============================================================================
# set linker options
# ==============================================================================
SET (CMAKE_SHARED_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
SET (CMAKE_EXE_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
