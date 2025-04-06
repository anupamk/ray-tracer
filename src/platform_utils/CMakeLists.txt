# ==============================================================================
# compile 'rt_platform_utils' library.
#
# this is a kind of 'low-level' library, using facilities exported by
# platform specific c-library f.e. 'mmap', 'fstat', 'threads' etc.
#
# the 'interface' to this library is defined in the header file, while
# platform specific implementation is in the '.cpp' file.
#
# choose the right implementation file based on the detected
# host-system.
#
# note
# ====
# the implemenation in the '.cpp' file can be broken into platform
# independent, and platform dependent pieces. for reasons of
# expediency, we just duplicate the whole implementation.
#
# is that good ? probably not. but it is OK.
# ==============================================================================
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")

  set(RT_FILE_UTILS_IMPL_FILENAME mmapped_file_reader_darwin.cpp)
  set(RT_THREAD_UTILS_IMPL_FILENAME thread_utils_darwin.cpp)

elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")

  set(RT_FILE_UTILS_IMPL_FILENAME mmapped_file_reader_linux.cpp)
  set(RT_THREAD_UTILS_IMPL_FILENAME thread_utils_linux.cpp)

else()

  # ----------------------------------------------------------------------------
  # fatal error. how did we get this far in the build process ?
  message("Error: Unsupported platform:'${CMAKE_HOST_SYSTEM_NAME}'for 'rt_file_utils'")
  message(FATAL_ERROR "die die die !!!")

endif()

# ==============================================================================
# the file utils library
add_library(rt_file_utils SHARED
  mmapped_file_reader.hpp
  ${RT_FILE_UTILS_IMPL_FILENAME})

target_link_libraries(rt_file_utils

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils)

augment_execution_profiling(rt_file_utils)

target_include_directories(rt_file_utils PUBLIC ${CMAKE_SOURCE_DIR}/src)

# ==============================================================================
# thread utils library
add_library(rt_thread_utils SHARED
  thread_utils.hpp
  ${RT_THREAD_UTILS_IMPL_FILENAME})

target_link_libraries(rt_thread_utils

  # ----------------------------------------------------------------------------
  # our libraries
  PRIVATE common_utils)

target_include_directories(rt_thread_utils PUBLIC ${CMAKE_SOURCE_DIR}/src)
