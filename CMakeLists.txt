# ------------------------------------------------------------------------------
# minimum cmake version required
cmake_minimum_required(VERSION 3.20...4.0)

# ------------------------------------------------------------------------------
# only 'Linux' or 'Darwin' systems are supported
if (NOT
    ((${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux") OR
      (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    ))
  message("unsupported platform: ${CMAKE_HOST_SYSTEM_NAME}")
  message(FATAL_ERROR "die die die !!!")
endif()

# ------------------------------------------------------------------------------
# what release type we want ? supported ones are:
#    - 'Debug'
#    - 'Release'
#    - 'RelWithDebInfo'
#    - 'MinSizeRel'
# pick one.
set(CMAKE_BUILD_TYPE Release)

# ------------------------------------------------------------------------------
# what compiler toolchain we want to use ? supported ones are:
#    - "LLVM", or
#    - "GNU"
# pick one
set(USE_TOOLCHAIN "LLVM")

# ==============================================================================
# with the basics done, everything else should be pretty much
# automated.
#
# lets just get on with it.
# ==============================================================================

# ------------------------------------------------------------------------------
# put all cmake modules together instead of they being scattered all
# over the place.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if (("${USE_TOOLCHAIN}" STREQUAL "LLVM") AND
    ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin"))

  # ----------------------------------------------------------------------------
  # llvm+darwin
  set(CMAKE_TOOLCHAIN_FILE ConfigureLLVMForDarwin)

elseif (("${USE_TOOLCHAIN}" STREQUAL "GNU") AND
    ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin"))

  # ----------------------------------------------------------------------------
  # gnu+darwin
  set(CMAKE_TOOLCHAIN_FILE ConfigureGNUForDarwin)

elseif (("${USE_TOOLCHAIN}" STREQUAL "LLVM") AND
    ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux"))

  # ----------------------------------------------------------------------------
  # llvm+linux
  set(CMAKE_TOOLCHAIN_FILE ConfigureLLVMForLinux)

elseif (("${USE_TOOLCHAIN}" STREQUAL "GNU") AND
    ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux"))

  # ----------------------------------------------------------------------------
  # gnu+linux
  set(CMAKE_TOOLCHAIN_FILE ConfigureGNUForLinux)

else()

  # ----------------------------------------------------------------------------
  # fatal-error: unsupported combination
  message("Error: Unsupported Configuration")
  message("Toolchain: ${USE_TOOLCHAIN}, system-name: ${CMAKE_HOST_SYSTEM_NAME}")
  message(FATAL_ERROR "die die die !!!")

endif()

message("tool-chain-file:'${CMAKE_TOOLCHAIN_FILE}', system-name: '${CMAKE_HOST_SYSTEM_NAME}'")
message("using '${USE_TOOLCHAIN}' toolchain in '${CMAKE_BUILD_TYPE}' mode")

# ------------------------------------------------------------------------------
# project name
project(the-ray-tracer
  LANGUAGES CXX)

# ------------------------------------------------------------------------------
# enable testing
include(CTest)
enable_testing()

# ------------------------------------------------------------------------------
# colorless + verbose builds
set(CMAKE_COLOR_MAKEFILE OFF)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# ------------------------------------------------------------------------------
# create the compilation-database (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# 3rd-party code (as submodules) making up this project. these are:
#   - doctest         : for tests
#   - concurrentqueue : concurrent mpmc queue
add_subdirectory(3rd-party)

# ------------------------------------------------------------------------------
# enable (ON) || disable (OFF) execution profiling
set(ENABLE_EXECUTION_PROFILING ON)

# ------------------------------------------------------------------------------
# tracy : realtime high-resolution profiler
if (ENABLE_EXECUTION_PROFILING)
  # ----------------------------------------------------------------------------
  # configure tracy features
  set(TRACY_ENABLE ON CACHE BOOL "enable profiling")
  set(TRACY_CALLSTACK ON CACHE BOOL "collect call stacks")

  # ----------------------------------------------------------------------------
  # for TracyClient or alias Tracy::TracyClient
  add_subdirectory(3rd-party/tracy)
endif()

# ------------------------------------------------------------------------------
# augment an executable or library target with tracy-execution-profiler
# artifacts
function (augment_execution_profiling EXECUTABLE_OR_LIB)

  if (ENABLE_EXECUTION_PROFILING)

    target_include_directories(${EXECUTABLE_OR_LIB}
      PRIVATE TracyClient)

    target_link_libraries(${EXECUTABLE_OR_LIB}
      PRIVATE TracyClient)

    target_compile_definitions(${EXECUTABLE_OR_LIB}
      PRIVATE TRACY_ENABLE
      PRIVATE TRACY_CALLSTACK)

  endif()

endfunction()

# ------------------------------------------------------------------------------
# project top-level
#    - 'src'    : all sources within this sub-directory
add_subdirectory(src)

# ------------------------------------------------------------------------------
# all assets (textures, OBJ's etc) within this sub-directory
add_subdirectory(assets)
