# ------------------------------------------------------------------------------
# ray tracer specific utilities
add_library(rt_utils SHARED
  badge.hpp
  constants.hpp
  execution_profiler.hpp
  utils.hpp)

set_target_properties(rt_utils
  PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(rt_utils

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils)

augment_execution_profiling(rt_utils)

target_include_directories(rt_utils PUBLIC ${CMAKE_SOURCE_DIR}/src)
