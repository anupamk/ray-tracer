# -*- cmake -*-

# ------------------------------------------------------------------------------
# the space at the beginning of each line is significant !
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# include-what-you-use: the right amount of includes in sources for
# FEARLESS development !
#
# see: https://github.com/include-what-you-use/include-what-you-use
# for some more information, and the reasons why this becomes useful.
set(ENABLE_INCLUDE_WHAT_YOU_USE OFF)

if (ENABLE_INCLUDE_WHAT_YOU_USE)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use;-Xiwyu;--comment_style=none;")
endif()

# ------------------------------------------------------------------------------
# for 'RAYTRACER_C_COMMON_FLAGS'
string (CONCAT RAYTRACER_C_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -Wextra"
  " -fsanitize=undefined,address"
  " -std=c11"                   # _C_FLAGS_
  " -pipe"
  " -fno-color-diagnostics"
  " -Wstrict-prototypes"
  " -Wold-style-definition"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RAYTRACER_C_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RAYTRACER_C_REL_FLAGS
  " -O3"
  " -march=native"

  # add more _CFLAGS_ there ↑
)

# ------------------------------------------------------------------------------
# set clang options
SET (CMAKE_C_COMPILER                  "/usr/bin/clang")
SET (CMAKE_C_FLAGS                     "${RAYTRACER_C_COMMON_FLAGS} ${CMAKE_C_FLAGS}")
SET (CMAKE_C_FLAGS_DEBUG               "${CMAKE_C_FLAGS_DEBUG}")
SET (CMAKE_C_FLAGS_MINSIZEREL          "${RAYTRACER_C_COMMON_FLAGS} ${RAYTRACER_C_MIN_SIZE_REL_FLAGS} ${CMAKE_C_FLAGS_MINSIZEREL}")
SET (CMAKE_C_FLAGS_RELEASE             "${RAYTRACER_C_COMMON_FLAGS} ${RAYTRACER_C_REL_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO      "${RAYTRACER_C_COMMON_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}")

# ------------------------------------------------------------------------------
# set g++ options
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# for 'CMAKE_CXX_FLAGS_INIT'
string (CONCAT RAYTRACER_CXX_COMMON_FLAGS
  " -Wall"
  " -Werror"
  " -std=c++17"                 # _CXX_FLAGS_
  " -pipe"
  " -fno-color-diagnostics"

  # add more _CXXFLAGS_ there ↑
)

string (CONCAT RAYTRACER_CXX_MIN_SIZE_REL_FLAGS
  " -Os"

  # add more _CFLAGS_ there ↑
)

string (CONCAT RAYTRACER_CXX_REL_FLAGS
  " -O3"
  " -march=native"

  # add more _CFLAGS_ there ↑
)

# ------------------------------------------------------------------------------
# set clang++ options
SET (CMAKE_CXX_COMPILER                  "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                     "${CMAKE_CXX_FLAGS}")
SET (CMAKE_CXX_FLAGS_DEBUG               "${CMAKE_CXX_FLAGS_DEBUG}")
SET (CMAKE_CXX_FLAGS_MINSIZEREL          "${RAYTRACER_CXX_COMMON_FLAGS} ${RAYTRACER_CXX_MIN_SIZE_REL_FLAGS} ${CMAKE_CXX_FLAGS_MINSIZEREL}")
SET (CMAKE_CXX_FLAGS_RELEASE             "${RAYTRACER_CXX_COMMON_FLAGS} ${RAYTRACER_CXX_REL_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO      "${RAYTRACER_CXX_COMMON_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

# ------------------------------------------------------------------------------
# set linker and other options
SET (CMAKE_AR                   "/usr/bin/llvm-ar")
SET (CMAKE_LINKER               "/usr/bin/llvm-link")
SET (CMAKE_NM                   "/usr/bin/llvm-nm")
SET (CMAKE_OBJDUMP              "/usr/bin/llvm-objdump")
SET (CMAKE_RANLIB               "/usr/bin/llvm-ranlib")
