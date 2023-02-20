# ------------------------------------------------------------------------------
# HEADER ONLY common utilities f.e. logging, assertions, benchmark
# etc. etc.
add_library(common_utils INTERFACE)
target_include_directories(common_utils INTERFACE ${CMAKE_SOURCE_DIR}/src)
