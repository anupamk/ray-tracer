# ------------------------------------------------------------------------------
# minimum cmake version required
cmake_minimum_required(VERSION 3.20)

# ------------------------------------------------------------------------------
# only support "Linux" platform (for now)
if (NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
  message("unsupported platform: ${CMAKE_HOST_SYSTEM_NAME}")
  message(FATAL_ERROR "die die die !!!")
endif()

# ------------------------------------------------------------------------------
# possible options 'Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel'
set(CMAKE_BUILD_TYPE Release)

# ------------------------------------------------------------------------------
# put all cmake modules together instead of they being scattered all
# over the place
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ------------------------------------------------------------------------------
# setup the toolchain, options 'LLVM', 'GNU'
set (RT_TOOLCHAIN "LLVM")

if ("${RT_TOOLCHAIN}" STREQUAL "LLVM")
  set(CMAKE_TOOLCHAIN_FILE ConfigureLLVM)
elseif ("${RT_TOOLCHAIN}" STREQUAL "GNU")
  set(CMAKE_TOOLCHAIN_FILE ConfigureGNU)
else()
  message(SEND_ERROR "Aieee !!! Unknown toolchain '${RT_TOOLCHAIN}'")
  message(SEND_ERROR "Expected 'LLVM' or 'GNU'")
  message(FATAL_ERROR "die die die")
endif()

message("Using '${RT_TOOLCHAIN}' toolchain in '${CMAKE_BUILD_TYPE}' mode")

# ------------------------------------------------------------------------------
# project name
project(the-ray-tracer
  LANGUAGES CXX)

# ------------------------------------------------------------------------------
# enable testing
include(CTest)
enable_testing()

# ------------------------------------------------------------------------------
# colorless verbose builds
set(CMAKE_COLOR_MAKEFILE 0)
set(CMAKE_VERBOSE_MAKEFILE ON)

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

  add_subdirectory(3rd-party/tracy)
endif()

# ------------------------------------------------------------------------------
# augment a library or executable target with tracy-execution-profiler
# artifacts
function (augment_execution_profiling EXEC_OR_LIB)

  if (ENABLE_EXECUTION_PROFILING)

    target_include_directories(${EXEC_OR_LIB}
      PRIVATE TracyClient)

    target_link_libraries(${EXEC_OR_LIB}
      PRIVATE TracyClient)

    target_compile_definitions(${EXEC_OR_LIB}
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
