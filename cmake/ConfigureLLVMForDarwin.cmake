# -*- cmake -*-

# ------------------------------------------------------------------------------
# the space at the beginning of each line is significant !
# ------------------------------------------------------------------------------

# ==============================================================================
# setup the llvm-tool-chain binaries
# ==============================================================================
SET (CMAKE_PREFIX_PATH     "/opt/homebrew")
SET (LLVM_BIN_DIR          "${CMAKE_PREFIX_PATH}/Cellar/llvm/20.1.2/bin/")
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
string (CONCAT C_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c11"                   # _C_FLAGS_
  " -pipe"
  " -fno-color-diagnostics"
  " -Wstrict-prototypes"
  " -Wold-style-definition"

  # add more _CFLAGS_ there ↑
)

string (CONCAT C_REL_FLAGS
  " -O3"
  " -march=native"

  # add more _CFLAGS_ there ↑
)

string (CONCAT C_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

SET (CMAKE_C_FLAGS                     "${C_COMMON_FLAGS} ${CMAKE_C_FLAGS}")
SET (CMAKE_C_FLAGS_DEBUG               "${CMAKE_C_FLAGS_DEBUG}")
SET (CMAKE_C_FLAGS_MINSIZEREL          "${C_COMMON_FLAGS} ${C_MIN_SIZE_REL_FLAGS} ${CMAKE_C_FLAGS_MINSIZEREL}")
SET (CMAKE_C_FLAGS_RELEASE             "${C_COMMON_FLAGS} ${C_REL_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO      "${C_COMMON_FLAGS} ${C_FLAGS_RELWITHDEBINFO}")

# ==============================================================================
# set clang++ options
# ==============================================================================
string (CONCAT CXX_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c++17"                 # _CXX_FLAGS_
  " -pipe"
  " -fno-color-diagnostics"

  # add more _CXXFLAGS_ there ↑
)

string (CONCAT CXX_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

string (CONCAT CXX_REL_FLAGS
  " -O3"
  " -march=native"

  # add more _CFLAGS_ there ↑
)

SET (CMAKE_CXX_FLAGS                     "${CXX_COMMON_FLAGS}")
SET (CMAKE_CXX_FLAGS_DEBUG               "${CXX_COMMON_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
SET (CMAKE_CXX_FLAGS_MINSIZEREL          "${CXX_MIN_SIZE_REL_FLAGS}")
SET (CMAKE_CXX_FLAGS_RELEASE             "${CXX_REL_FLAGS}")

# ==============================================================================
# set linker options
# ==============================================================================
SET (CMAKE_SHARED_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
SET (CMAKE_EXE_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
