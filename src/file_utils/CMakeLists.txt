# ------------------------------------------------------------------------------
# file specific operations
add_library(rt_file_utils SHARED
  mmapped_file_reader.hpp
  mmapped_file_reader.cpp)

target_link_libraries(rt_file_utils

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils)

augment_execution_profiling(rt_file_utils)

target_include_directories(rt_file_utils PUBLIC ${CMAKE_SOURCE_DIR}/src)
