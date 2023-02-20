# ------------------------------------------------------------------------------
# ray tracer specific utilities
add_library(rt_utils SHARED
  badge.hpp
  constants.hpp
  execution_profiler.hpp
  mmapped_file_reader.hpp
  mmapped_file_reader.cpp
  utils.hpp)

target_link_libraries(rt_utils
  PRIVATE common_utils)

augment_execution_profiling(rt_utils)

target_include_directories(rt_utils PUBLIC ${CMAKE_SOURCE_DIR}/src)

