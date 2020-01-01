project (doctest)

# define a header-only library target
add_library(doctest INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
