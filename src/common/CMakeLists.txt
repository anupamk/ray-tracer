# ------------------------------------------------------------------------------
# HEADER ONLY common utilities f.e. logging, assertions, benchmark
# etc. etc.
add_library(common_utils INTERFACE)

set_target_properties(common_utils
    PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)

target_include_directories(common_utils INTERFACE ${CMAKE_SOURCE_DIR}/src)
