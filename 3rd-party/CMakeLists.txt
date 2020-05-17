project (3rdparty-libs)

# define a header-only library target
add_library(doctest INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/doctest/include)

# define a header-only library target
add_library(concurrentqueue INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(concurrentqueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/concurrentqueue)
