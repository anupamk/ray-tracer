# -*- cmake -*-

# ------------------------------------------------------------------------------
# the space at the beginning of each line is significant !
# ------------------------------------------------------------------------------

# ==============================================================================
# setup the gnu-tool-chain binaries
# ==============================================================================
SET (GCC_BIN_DIR                    "/opt/homebrew/Cellar/gcc/14.2.0_1/bin")
SET (CMAKE_C_COMPILER               "${GCC_BIN_DIR}/gcc-14")
SET (CMAKE_CXX_COMPILER             "${GCC_BIN_DIR}/g++-14")
SET (CMAKE_AR                       "${GCC_BIN_DIR}/gcc-ar-14")
SET (CMAKE_NM                       "${GCC_BIN_DIR}/gcc-nm-14")
SET (CMAKE_OBJDUMP                  "${GCC_BIN_DIR}/llvm-objdump")
SET (CMAKE_RANLIB                   "${GCC_BIN_DIR}/gcc-ranlib-14")

# hopefully not much of a change below this line is needed
# ------------------------------------------------------------------------------

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
SET (CMAKE_SHARED_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
SET (CMAKE_EXE_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
