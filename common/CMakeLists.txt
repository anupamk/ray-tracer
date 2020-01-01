project (common)

# define a header-only library target
add_library(common INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
